// ======================================================================== //
// Copyright SCI Institute, University of Utah, 2018
// ======================================================================== //
#include "properties.h"

#include <imgui.h>
#include <imgui_glfw_impi.h>
#include "widgets/TransferFunctionWidget.h"

// ======================================================================== //
//
// ======================================================================== //
using namespace ospcommon;

// ======================================================================== //
//
// ======================================================================== //
void viewer::CameraProp::Init(OSPCamera camera, 
                              const viewer::CameraProp::Type& t) 
{
  type = t;
  if (camera == nullptr) { throw std::runtime_error("empty camera found"); }
  self = camera;
}
void viewer::CameraProp::Draw()
{
}
bool viewer::CameraProp::Commit()
{
  bool update = false;
  if (pos.update()) {
    ospSetVec3f(self, "pos", (osp::vec3f &) pos.ref()); 
    update = true;
  }
  if (dir.update()) {
    ospSetVec3f(self, "dir", (osp::vec3f &) dir.ref());
    update = true;
  }
  if (up.update()) {
    ospSetVec3f(self, "up", (osp::vec3f &) up.ref());
    update = true;
  }
  if (aspect.update()) {
    ospSet1f(self, "aspect", aspect.ref());
    update = true;
  }
  if (type == Perspective) {
    if (fovy.update()) {
      ospSet1f(self, "fovy", std::min(fovy.ref(), 180.f));
      update = true;
    }
  }
  else if (type == Orthographic) {
    if (height.update()) {
      ospSet1f(self, "height", height.ref());
      update = true;
    }
  }
  if (update)
    ospCommit(self);
  return update;
}

// ======================================================================== //
//
// ======================================================================== //
viewer::LightProp::~LightProp() 
{
  if (self != nullptr) ospRelease(self);
}
void viewer::LightProp::Init(const std::string& s,
                             const std::string& r,
                             const size_t i)
{
  type = s; 
  renderer = r;
  self = ospNewLight2(r.c_str(), s.c_str());
  name = std::to_string(i);
}
void viewer::LightProp::Draw()
{
  ImVec4 picked_color = ImColor(imgui_C.x, imgui_C.y, imgui_C.z, 1.f);
  if (ImGui::ColorEdit4(("color##" + name).c_str(),
                        (float *) &picked_color,
                        ImGuiColorEditFlags_NoAlpha |
                        ImGuiColorEditFlags_NoInputs |
                        ImGuiColorEditFlags_NoLabel |
                        ImGuiColorEditFlags_AlphaPreview |
                        ImGuiColorEditFlags_NoOptions |
                        ImGuiColorEditFlags_NoTooltip)) {
    imgui_C.x = picked_color.x;
    imgui_C.y = picked_color.y;
    imgui_C.z = picked_color.z;
    C = imgui_C;
  }
  ImGui::SameLine();
  ImGui::Text((type + "-" + name).c_str());
  if (ImGui::SliderFloat(("intensity##" + name).c_str(), &imgui_I, 
                         0.f, 100000.f, "%.3f", 5.0f)) {
    I = imgui_I;    
  }
  /* distant light */
  if (type == "distant" || type == "directional" ||
      type == "DistantLight" || type == "DirectionalLight") {
    if (ImGui::SliderFloat3(("direction##" + name).c_str(),
                            &imgui_D.x, -1.f, 1.f)) {
      D = imgui_D;
    }
  }
}
bool viewer::LightProp::Commit()
{
  bool update = false;
  if (I.update()) {
    ospSet1f(self, "intensity", I.ref());
    update = true;
  }
  if (C.update()) {
    ospSetVec3f(self, "color", (osp::vec3f &)C.ref());
    update = true;
  }
  if(D.update()) {
    ospSetVec3f(self, "direction", (osp::vec3f &)D.ref());
    update = true;
  }
  if (update) { ospCommit(self); }
  return update;
}
viewer::LightListProp::~LightListProp() {
  if (self != nullptr) ospRelease(self);
  for (auto& p : prop) { delete p; }
}

void viewer::LightListProp::Append(const std::string& s,
                                   const std::string& r)
{
  LightProp* p = new LightProp();
  p->Init(s, r, this->Size());
  data.push_back(**p);
  prop.push_back(p);
}
void viewer::LightListProp::Finalize() 
{
  self = ospNewData(data.size(), OSP_OBJECT, data.data(),
                    OSP_DATA_SHARED_BUFFER);
  ospCommit(self);
}
void viewer::LightListProp::Draw() 
{
  for (size_t i = 0; i < this->Size(); ++i) {
    prop[i]->Draw();
  }
}
bool viewer::LightListProp::Commit() 
{
  bool update = false;
  for (size_t i = 0; i < this->Size(); ++i) {
    if (prop[i]->Commit()) { update = true; }
  }
  return update;
}

// ======================================================================== //
//
// ======================================================================== //
viewer::RendererProp::RendererProp(CameraProp& c, LightListProp& l)
  : camera(c), lights(l)
{}
void viewer::RendererProp::Init(OSPRenderer renderer, 
                                const viewer::RendererProp::Type& t) 
{
  type = t;
  self = renderer;
}
void viewer::RendererProp::Draw()
{
  if (ImGui::Checkbox("autoEpsilon", &imgui_autoEpsilon)) {
    autoEpsilon = imgui_autoEpsilon;
  }
  if (ImGui::SliderInt("maxDepth", &imgui_maxDepth, 0, 100, "%.0f")) {
    maxDepth = imgui_maxDepth;
  }
  if (ImGui::Checkbox("shadowsEnabled", &imgui_shadowsEnabled)) {
    shadowsEnabled = imgui_shadowsEnabled;
  }
  if (ImGui::SliderInt("spp", &imgui_spp, 0, 100, "%.0f")) {
    spp = imgui_spp;
  }
  if (ImGui::SliderInt("aoSamples", &imgui_aoSamples, 0, 100, "%.0f")) {
    aoSamples = imgui_aoSamples;
  }
  if (ImGui::SliderFloat("aoDistance", &imgui_aoDistance, 
                         0.f, 1e20, "%.3f", 5.0f)) {
    aoDistance = imgui_aoDistance;
  }
  if (ImGui::Checkbox("aoTransparencyEnabled", 
                      &imgui_aoTransparencyEnabled)) {
    aoTransparencyEnabled = imgui_aoTransparencyEnabled;
  }
  if (ImGui::Checkbox("oneSidedLighting", 
                      &imgui_oneSidedLighting)) {
    oneSidedLighting = imgui_oneSidedLighting;
  }
}
bool viewer::RendererProp::Commit()
{
  bool update = false;
  if (aoSamples.update()) {
    ospSet1i(self, "aoSamples", aoSamples.ref());
    update = true;
  }
  if (aoDistance.update()) {
    ospSet1f(self, "aoDistance", aoDistance.ref());
    update = true;
  }
  if (shadowsEnabled.update()) {
    ospSet1i(self, "shadowsEnabled", shadowsEnabled.ref());
    update = true;
  }
  if (maxDepth.update()) {
    ospSet1i(self, "maxDepth", maxDepth.ref());
    update = true;
  }
  if (aoTransparencyEnabled.update()) {
    ospSet1i(self, "aoTransparencyEnabled", aoTransparencyEnabled.ref());
    update = true;
  }
  if (spp.update()) {
    ospSet1i(self, "spp", spp.ref());
    update = true;
  }
  if (autoEpsilon.update()) {
    ospSet1i(self, "autoEpsilon", autoEpsilon.ref());
    update = true;
  }
  if (epsilon.update()) {
    ospSet1f(self, "epsilon", epsilon.ref());
    update = true;
  }
  if (minContribution.update()) {
    ospSet1f(self, "minContribution", minContribution.ref());
    update = true;
  }
  ospSetObject(self, "camera", *camera);
  ospSetData(self, "lights", *lights);
  ospCommit(self);
  return update;
}

// ======================================================================== //
//
// ======================================================================== //
void viewer::TransferFunctionProp::Init()
{
  using tfn::tfn_widget::TransferFunctionWidget;
  if (self != nullptr) {
    widget = std::make_shared<TransferFunctionWidget>
      ([&](const std::vector<float> &c,
           const std::vector<float> &a,
           const std::array<float, 2> &r) 
       {
         lock.lock();
         colors = std::vector<float>(c);
         alphas = std::vector<float>(a);
         valueRange = vec2f(r[0], r[1]);
         doUpdate = true;
         lock.unlock();
       });
    widget->setDefaultRange(valueRange_default[0],
                            valueRange_default[1]);
  }
}
void viewer::TransferFunctionProp::Print()
{
  if ((!colors.empty()) && !(alphas.empty())) {
    const std::vector<float> &c = colors;
    const std::vector<float> &a = alphas;
    std::cout << std::endl
              << "static std::vector<float> colors = {" << std::endl;
    for (int i = 0; i < c.size() / 3; ++i) {
      std::cout << "    " << c[3 * i] << ", " << c[3 * i + 1] << ", "
                << c[3 * i + 2] << "," << std::endl;
    }
    std::cout << "};" << std::endl;
    std::cout << "static std::vector<float> opacities = {" << std::endl;
    for (int i = 0; i < a.size() / 2; ++i) {
      std::cout << "    " << a[2 * i + 1] << ", " << std::endl;
    }
    std::cout << "};" << std::endl << std::endl;
  }
}
void viewer::TransferFunctionProp::Draw(bool* p_open)
{
  if (self != nullptr) {
    if (widget->drawUI(p_open)) { widget->render(128); };
  }
}
bool viewer::TransferFunctionProp::Commit()
{
  bool update = false;
  if (doUpdate && lock.try_lock()) {
    doUpdate = false;
    OSPData colorsData = ospNewData(colors.size() / 3, 
                                    OSP_FLOAT3, 
                                    colors.data());
    ospCommit(colorsData);
    std::vector<float> o(alphas.size() / 2);
    for (int i = 0; i < alphas.size() / 2; ++i) {
      o[i] = alphas[2 * i + 1]; 
    }
    OSPData opacitiesData = ospNewData(o.size(), OSP_FLOAT, o.data());   
    ospCommit(opacitiesData);
    ospSetData(self, "colors", colorsData);
    ospSetData(self, "opacities", opacitiesData);
    if (valueRange.update())
      ospSetVec2f(self, "valueRange", (osp::vec2f&)valueRange.ref());
    ospCommit(self);
    ospRelease(colorsData);
    ospRelease(opacitiesData);
    lock.unlock();
    update = true;
  }
  return update;
}

