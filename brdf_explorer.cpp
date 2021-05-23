/// Copyright (c) 2021, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file brdf_explorer.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-04-18
///
///\brief

#include <circe/circe.h>

using namespace circe::gl;

class BRDFExplorer : public BaseApp {
public:
  BRDFExplorer() : BaseApp(1280, 720) {
    ponos::Path path(SRC_PATH);
    d_src = ponos::FileSystem::readFile(path + "brdf/d_ggx.frag");
    f_src = ponos::FileSystem::readFile(path + "brdf/f_schlick.frag");
    g_src = ponos::FileSystem::readFile(path + "brdf/g_smith_schlick_beckmann.frag");
    setupModel();
    buildShader();
  }

  void prepareFrame(const circe::gl::ViewportDisplay &display) override {
    circe::gl::BaseApp::prepareFrame(display);
    ImGuizmo::BeginFrame();
  }

  void render(circe::CameraInterface *camera) override {
    circe::gl::GraphicsDisplay::clearScreen(0.1f, 0.1f, 0.1f, 1.f);
    glEnable(GL_DEPTH_TEST);
    light_model.program.use();
    light_model.program.setUniform("proj", camera->getProjectionTransform());
    light_model.program.setUniform("view", camera->getViewTransform());
    light_model.program.setUniform("model", model.transform);
    light_model.draw();
    shader.use();
    shader.setUniform("projection", camera->getProjectionTransform());
    shader.setUniform("view", camera->getViewTransform());
    shader.setUniform("model", model.transform);
    shader.setUniform("cameraPosition", camera->getPosition());
    shader.setUniform("albedo", albedo);
    shader.setUniform("metallic", metallic);
    shader.setUniform("roughness", roughness);
    shader.setUniform("ao", ao);
    shader.setUniform("f0", f0);
    shader.setUniform("lightPosition", light_position);
    shader.setUniform("lightColor", light_color);
    if (use_ibl) {
      irradiance_map.bind(GL_TEXTURE0);
      prefilter_map.bind(GL_TEXTURE1);
      lut.bind(GL_TEXTURE2);
      shader.setUniform("irradianceMap", 0);
      shader.setUniform("prefilterMap", 1);
      shader.setUniform("brdfLUT", 2);
    }
    model.draw();
    gui();
    // gizmo
    ImGuizmo::SetRect(0, 0, this->app_->viewports[0].width, this->app_->viewports[0].height);
    circe::Gizmo::update(camera, light_model.transform, ImGuizmo::OPERATION::TRANSLATE);
    light_position = light_model.transform(ponos::point3());
    circe::Gizmo::draw(camera,
                       1,
                       {1000, 100},
                       {100, 100});

    if (display_enviroment)
      drawEnviroment(camera);
  }

  void setupModel() {
    model = circe::Shapes::icosphere({}, 1, 5, circe::shape_options::normal
        | circe::shape_options::uv);
    // light
    light_model = circe::Shapes::icosphere({}, 0.1, 1);
    light_model.transform = ponos::translate((ponos::vec3) light_position);
    std::vector<Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, "#version 440 core\n"
                                           "layout(location = 0) in vec3 position;\n"
                                           "uniform mat4 proj;"
                                           "uniform mat4 view;"
                                           "uniform mat4 model;"
                                           "void main() {\n"
                                           "    gl_Position = proj * view * model * vec4(position, 1.0);}");
    shaders.emplace_back(GL_FRAGMENT_SHADER, "#version 440 core\n"
                                             "out vec4 FragColor;\n"
                                             "void main(){FragColor = vec4(1.0,1.0,1.0,1.0);}\n");
    if (!light_model.program.link(shaders))
      std::cerr << light_model.program.err << std::endl;

    setupEnviroment();
  }

  void setupEnviroment() {
    // enviroment
    env_model = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}});
    std::vector<Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, "#version 440 core\n"
                                           "layout (location = 0) in vec3 aPos;\n"
                                           "uniform mat4 projection;\n"
                                           "uniform mat4 view;\n"
                                           "out vec3 localPos;\n"
                                           "void main() {\n"
                                           "    localPos = aPos;\n"
                                           "    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix\n"
                                           "    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);\n"
                                           "    gl_Position = clipPos.xyww;\n"
                                           "}");
    shaders.emplace_back(GL_FRAGMENT_SHADER, "#version 440 core\n"
                                             "out vec4 FragColor;\n"
                                             "in vec3 localPos;"
                                             "in vec3 TexCoords;\n"
                                             "uniform samplerCube environmentMap;\n"
                                             "void main(){\n"
                                             "  vec3 envColor = texture(environmentMap, localPos).rgb;"
                                             "  envColor = envColor / (envColor + vec3(1.0));\n"
                                             "  envColor = pow(envColor, vec3(1.0/2.2)); \n"
                                             "  FragColor = vec4(envColor, 1.0);"
                                             "}");
    if (!env_model.program.link(shaders))
      std::cerr << env_model.program.err << std::endl;

    env_map = circe::gl::Texture::fromFile("/home/filipecn/Desktop/hdr/Ueno-Shrine/03-Ueno-Shrine_3k.hdr",
                                           circe::texture_options::equirectangular |
                                               circe::texture_options::hdr,
                                           circe::texture_options::cubemap);
    env_map.bind();
    Texture::View view(GL_TEXTURE_CUBE_MAP);
    view[GL_TEXTURE_MIN_FILTER] = GL_LINEAR_MIPMAP_LINEAR;
    view.apply();
    env_map.generateMipmap();
    // compute IBL textures
    irradiance_map = circe::gl::IBL::irradianceMap(env_map, {32, 32});
    prefilter_map = circe::gl::IBL::preFilteredEnvironmentMap(env_map, {128, 128});
    lut = circe::gl::IBL::brdfIntegrationMap({512, 512});
    irradiance_tex = circe::gl::Texture::fromTexture(irradiance_map, circe::texture_options::equirectangular);
    env_tex = circe::gl::Texture::fromTexture(env_map, circe::texture_options::equirectangular);
    prefilter_tex = circe::gl::Texture::fromTexture(prefilter_map, circe::texture_options::equirectangular);
  }

  void drawEnviroment(circe::CameraInterface *camera) {
    env_map.bind(GL_TEXTURE0);
    glDepthFunc(GL_LEQUAL);
    env_model.program.use();
    auto m = camera->getViewTransform().matrix();
    m[0][3] = m[1][3] = m[2][3] = 0;
    env_model.program.setUniform("projection", camera->getProjectionTransform());
    env_model.program.setUniform("view", camera->getViewTransform());
    env_model.program.setUniform("environmentMap", 0);
    env_model.draw();
    glDepthFunc(GL_LESS);
  }

  void buildShader() {
    ponos::Path path(SHADER_CODE_PATH);
    ponos::Str frag_src = ponos::FileSystem::readFile(path + "pbr_header.frag");
    // choose BRDFs
    frag_src += d_src;
    frag_src += f_src;
    frag_src += g_src;
    // finish fragment shader
    frag_src += "void main(){\n";
    frag_src += ponos::FileSystem::readFile(path + "main_header.frag");
    frag_src += "{\n";
    frag_src += ponos::FileSystem::readFile(path + "light_iteration.frag");
    frag_src += "}\n";
    if (use_ibl)
      frag_src += ponos::FileSystem::readFile(path + "ambient_ibl.frag");
    else
      frag_src += ponos::FileSystem::readFile(path + "ambient_light.frag");
    frag_src += ponos::FileSystem::readFile(path + "main_footer.frag");
    frag_src += "}\n";
//    std::cerr << frag_src << std::endl;

    if (!vertex_shader.compile(path + "pbr.vert", GL_VERTEX_SHADER))
      std::cerr << vertex_shader.err << std::endl;
    if (!fragment_shader.compile(frag_src.str(), GL_FRAGMENT_SHADER))
      std::cerr << fragment_shader.err << std::endl;

    shader.destroy();
    shader.attach(vertex_shader);
    shader.attach(fragment_shader);
    if (!shader.link())
      std::cerr << shader.err << std::endl;
    shader.use();
  }

  void gui() {
    ImGui::Begin("Controls");
    ImGui::Text("IBL");
    if (ImGui::Checkbox("enable", &use_ibl))
      buildShader();
    ImGui::SameLine();
    ImGui::Checkbox("render", &display_enviroment);
    ImGui::Text("Display");
    ImGui::Separator();
    ImGui::Checkbox("BRDF LUT", &view_lut);
    ImGui::Checkbox("Environment Map", &view_env_map);
    ImGui::Checkbox("Irradiance Map", &view_irradiance_map);
    ImGui::Checkbox("Prefilter Map", &view_prefilter_map);
    ImGui::Text("Material");
    ImGui::ColorEdit3("albedo", &albedo[0]);
    ImGui::SliderFloat("roughness", &roughness, 0.0, 1.0);
    ImGui::SliderFloat("metallic", &metallic, 0.0, 1.0);
    ImGui::SliderFloat("ao", &ao, 0.0, 1.0);
    ImGui::Separator();
    ImGui::Text("NDF");
    static int ndf_option = 0;
    int old_option = ndf_option;
    if (ImGui::Combo("NDF", (int *) &ndf_option,
                     "GGX\0Blinn-Phong\0Beckmann\0CUSTOM\0")
        && ndf_option != old_option)
      loadSource(0, ndf_option);
    static int g_option = 0;
    old_option = g_option;
    if (ImGui::Combo("G", (int *) &g_option,
                     "Smith Schlick Beckmann\0"
                     "Cook-Torrance\0"
                     "Smith GGX\0"
                     "Neumann\0"
                     "Implicit\0"
                     "Kelemen\0"
                     "CUSTOM\0")
        && g_option != old_option)
      loadSource(1, g_option);
    static int f_option = 0;
    old_option = f_option;
    if (ImGui::Combo("F", (int *) &f_option,
                     "None\0"
                     "Schlick\0"
                     "Cook Torrance\0"
                     "CUSTOM\0")
        && g_option != old_option)
      loadSource(2, f_option);

    ImGui::SliderFloat("F0", &f0, 0.0, 1.0);
    ImGui::End();

    if (view_lut)
      guiTextureView(lut, "BRDF LUT");
    if (view_irradiance_map)
      guiTextureView(irradiance_tex, "Irradiance Map");
    if (view_env_map)
      guiTextureView(env_tex, "Environment Map");
    if (view_prefilter_map)
      guiTextureView(prefilter_tex, "Prefilter Map");
  }

  static void guiTextureView(const circe::gl::Texture &texture, const std::string &title) {
    ImGui::Begin(title.c_str());
    if (texture.target() == GL_TEXTURE_CUBE_MAP)
      glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture.textureObjectId());
    texture.bind(GL_TEXTURE0);
    auto texture_id = texture.textureObjectId();
    ImGui::Image((void *) (intptr_t) (texture_id),
                 {static_cast<float>(texture.size().width), static_cast<float>(texture.size().height)},
                 {0, 1},
                 {1, 0});
    ImGui::End();
  }

  void loadSource(int f, int i) {
    ponos::Path path(SRC_PATH);
    static std::vector<std::string> ndfs = {
        "brdf/d_ggx.frag",
        "brdf/d_blinn_phong.frag",
        "brdf/d_beckmann.frag",
    };
    static std::vector<std::string> gs = {
        "brdf/g_smith_schlick_beckmann.frag",
        "brdf/g_cook_torrance.frag",
        "brdf/g_smith_ggx.frag",
        "brdf/g_neumann.frag",
        "brdf/g_implicit.frag",
        "brdf/g_kelemen.frag",
    };
    static std::vector<std::string> fs = {
        "brdf/f_none.frag",
        "brdf/f_schlick.frag",
        "brdf/f_cook_torrance.frag",
    };
    if (f == 0) {
      if (i < ndfs.size())
        d_src = ponos::FileSystem::readFile(path + ndfs[i]);
    } else if (f == 1) {
      if (i < gs.size())
        g_src = ponos::FileSystem::readFile(path + gs[i]);
    } else {
      if (i < fs.size())
        f_src = ponos::FileSystem::readFile(path + fs[i]);
    }
    buildShader();
  }

  // scene
  SceneModel model;
  // shader
  Shader vertex_shader;
  Shader fragment_shader;
  Program shader;
  // shader srcs
  std::string d_src;
  std::string g_src;
  std::string f_src;
  // shader uniforms
  ponos::vec3 albedo{0.4, 0., 0.};
  f32 metallic{0.5};
  f32 roughness{0.5};
  f32 ao{1.0};
  f32 f0{0.04};
  // light
  SceneModel light_model;
  ponos::point3 light_position{5, 5, 5};
  ponos::vec3 light_color{300, 300, 300};
  // env
  bool view_env_map{false};
  bool view_irradiance_map{false};
  bool view_prefilter_map{false};
  bool view_lut{false};
  bool display_enviroment{false};
  bool use_ibl{false};
  SceneModel env_model;
  circe::gl::Texture env_map, env_tex;
  circe::gl::Texture irradiance_map, irradiance_tex;
  circe::gl::Texture prefilter_map, prefilter_tex;
  circe::gl::Texture lut;

};

int main() {
  return BRDFExplorer().run();
}