// ======================================================================== //
// Copyright SCI Institute, University of Utah, 2018
// ======================================================================== //
#pragma once
#ifndef OSPRAY_PROPERTIES_H
#define OSPRAY_PROPERTIES_H

// ======================================================================== //
// This is the place where to involve ospray commits
// ======================================================================== //
namespace viewer { namespace widgets { bool Commit(); };};

/* ospray */
#include "ospray/ospray.h"
#include "ospcommon/vec.h"
#include "ospcommon/utility/TransactionalValue.h"
/* imgui */
#include "widgets/TransferFunctionWidget.h"
/* stl */
#include <vector>

// ======================================================================== //
//
// ======================================================================== //
#define TValue ospcommon::utility::TransactionalValue
#define Setter(field, name, type, value)                 \
private:                                                 \
  TValue<type> field{value};                             \
  type imgui_##field = field.ref();                      \
public:                                                  \
  void Set##name (const type& v) {                       \
    this->field = v;                                     \
    this->imgui_##field = v;                             \
  }

namespace viewer {

  // ====================================================================== //
  //
  // ====================================================================== //
  struct Prop { 
    Prop() {}
    virtual ~Prop() {}
    virtual void Draw() = 0; 
    virtual bool Commit() = 0; 
  };

  // ====================================================================== //
  //
  // ====================================================================== //
  class CameraProp : public Prop
  {
  private:
    OSPCamera self {nullptr};
  public:
    enum Type { Perspective, Orthographic, Panoramic } type;
  public:
    // ==== Set ===== //
    Setter(pos, Pos, ospcommon::vec3f, ospcommon::vec3f(0.f));
    Setter(dir, Dir, ospcommon::vec3f, ospcommon::vec3f(0.f, 0.f, 1.f));
    Setter(up, Up, ospcommon::vec3f, ospcommon::vec3f(0.f, 1.f, 0.f));
    Setter(nearClip, NearClip, float, 1e-6f);
    Setter(imageStart, ImageStart, ospcommon::vec2f, ospcommon::vec2f(0.f));
    Setter(imageEnd, ImageEnd, ospcommon::vec2f, ospcommon::vec2f(0.f));
    Setter(shutterOpen, ShutterOpen, float, 0.f);
    Setter(shutterClose, ShutterClose, float, 0.f);
    Setter(aspect, Aspect, float, 1.f);
    /* perspective camera */
    Setter(fovy, Fovy, float, 60.f);
    Setter(apertureRadius, ApertureRadius, float, 0.f);
    Setter(focusDistance, FocusDistance, float, 1.f);
    Setter(architectural, Architectural, bool, false);
    Setter(stereoMode, StereoMode, int, 0);
    Setter(interpupillaryDistance, InterpupillaryDistance, float, 0.0635f);
    /* orthographic camera */
    Setter(height, Height, float, 1.f);
  public:
    CameraProp() = default;
    OSPCamera& operator*() { return self; }
    void Init(OSPCamera camera, const Type& t);
    void Draw();
    bool Commit();
  };

  // ====================================================================== //
  //
  // ====================================================================== //
  class LightProp : public Prop 
  {
  private:
    OSPLight self {nullptr};
    std::string type, renderer, name;
  public:
    Setter(I, Intensity, float, 0.25f); 
    Setter(D, Direction, ospcommon::vec3f,
           ospcommon::vec3f(-1.f, 0.679f, -0.754f));
    Setter(C, Color, ospcommon::vec3f,
           ospcommon::vec3f(1.f, 1.f, 1.f));
    Setter(angularDiameter, AngularDiameter, float, 0.53f);
  public:
    LightProp() = default;
    ~LightProp();
    OSPLight& operator*() { return self; }
    void Init(const std::string& s,
              const std::string& r,
              const size_t i);
    void Draw();
    bool Commit();
  };
  class LightListProp : public Prop {
  private:
    std::vector<OSPLight>   data;
    std::vector<LightProp*> prop;
    OSPData self = nullptr;
  public:
    LightListProp() = default;
    ~LightListProp();
    OSPData&        operator*()       { return self; } 
    const OSPData&  operator*() const { return self; } 
    LightProp&       operator[](const size_t i) { return *(prop[i]); }
    const LightProp& operator[](const size_t i) const { return *(prop[i]); }
    size_t Size() { return data.size(); }
    void Append(const std::string& s, const std::string& r);
    void Finalize();
    void Draw();
    bool Commit();
  };

  // ====================================================================== //
  //
  // ====================================================================== //
  class RendererProp : public Prop
  {
  private:
    OSPRenderer self {nullptr};
    CameraProp& camera;
    LightListProp& lights;
  public:
    enum Type { Scivis, Pathtracer } type;
  public:
    // ==== renderer ===== //
    Setter(autoEpsilon, AutoEpsilon, bool, true);
    Setter(epsilon, Epsilon, float, 1e-6f);
    Setter(spp, Spp, int, 1);
    Setter(maxDepth, MaxDepth, int, 20);
    Setter(minContribution, MinContribution, float, 0.001f);
    Setter(varianceThreshold, VarianceThreshold, float, 0.f);
    Setter(bgColor, BgColor, ospcommon::vec4f, ospcommon::vec4f(0.f));
    // ==== scivis renderer ===== //
    Setter(shadowsEnabled, ShadowsEnabled, bool, false);
    Setter(aoSamples, AoSamples, int, 0);
    Setter(aoDistance, AoDistance, float, 1e20f);
    Setter(aoWeight, AoWeight, float, 0.f);
    Setter(aoTransparencyEnabled, AoTransparencyEnabled, bool, false);
    Setter(oneSidedLighting, OneSidedLighting, bool, true);
    // ==== pathtracer renderer ===== //
    Setter(rouletteDepth, RouletteDepth, int, 5);
    Setter(maxContribution, MaxContribution, float, ospcommon::inf);
  public:
    RendererProp(CameraProp& c, LightListProp& l);
    OSPRenderer& operator*() { return self; }
    void Init(OSPRenderer renderer, const Type& t);
    void Draw();
    bool Commit();
  };

  // ====================================================================== //
  //
  // ====================================================================== //
  class TransferFunctionProp : public Prop
  {
  private:
    OSPTransferFunction self {nullptr};
    bool doUpdate{false}; // no initial update
    std::shared_ptr<tfn::tfn_widget::TransferFunctionWidget> widget;
    std::mutex lock;
    std::vector<float> colors;
    std::vector<float> alphas;
    float valueRange_default[2] = {0.f, 1.f};
    TValue<ospcommon::vec2f> valueRange;
  public:
    TransferFunctionProp() = default;
    OSPTransferFunction& operator*() { return self; }
    void Create(OSPTransferFunction o, 
                const float a = 0.f, 
                const float b = 1.f)
    {
      self             = o;
      valueRange_default[0] = a;
      valueRange_default[1] = b;
    }
    void Init();
    void Draw(bool* p_open = NULL);
    void Draw() { Draw(NULL); };
    bool Commit();
    void Print();
  };
};
#undef Setter
#undef TValue
#endif//OSPRAY_PROPERTIES_H
