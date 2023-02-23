#pragma once

#include <assert.h> 
#include <shlwapi.h>
#include <dwmapi.h>
#include <initguid.h>
#include <QRect>
#include <QObject>
#include <ShObjIdl.h>
//#include <wrl/implements.h>
#include <QTimer>

DEFINE_GUID(CLSID_UIHostNoLaunch, 0x4CE576FA, 0x83DC, 0x4f88, 0x95, 0x1C, 0x9D, 0x07, 0x82, 0xB4, 0xE3, 0x76);
DEFINE_GUID(IID_ITipInvocation, 0x37c994e7, 0x432b, 0x4834, 0xa2, 0xf7, 0xdc, 0xe1, 0xf1, 0x3b, 0x83, 0x4b);
struct ITipInvocation : IUnknown {
  virtual HRESULT STDMETHODCALLTYPE Toggle(HWND wnd) = 0;
};

class TouchKeyboardManager : public QObject {
  Q_OBJECT

public:
  static TouchKeyboardManager* GetInstence();

  void ShowKeyBoard();
  void HideKeyBoard();
  void CloseKeyBoard();
  void SetKeyboardVisible(bool);
  void MoveFocusWidgetPos();
  void ResetFocusWidgetPos();
  void SetWidget(QWidget* widget, double heigth, bool move);
  bool IsKeyboardHide();
  void OnExit();

  bool LineEditHasFocusWhenFocusIn(QObject* target, QEvent* event);
  bool LineEditNoFocusWhenFocusIn(QObject* target, QEvent* event);
  bool LineEditNoFocusWhenMouseButtonRelease(QObject* target, QEvent* event);

private:
  TouchKeyboardManager();
  ~TouchKeyboardManager();
  QRect GetKeyBoardRect();
  bool IsKeyboardOpen();
  void OpenKeyBoard();
  bool JudgeKeyBoardVisivle();
  void StartTimerListen();
  void StopTimerListen();
  bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
  void ListenTimeOut();

private:
  Microsoft::WRL::ComPtr<IFrameworkInputPane> framework_input_pane_;
  Microsoft::WRL::ComPtr<ITipInvocation> tip_invocation_;
  QWidget* current_widget_ = nullptr;
  QPoint focus_object_widget_pos_;
  bool keyboard_visible_ = false;
  bool is_move_ = true;
  bool change_focus_widget_pos_ = false;
  double interval_ = 0.0;
  DWORD cookie_ = 0;
  int keyboard_height = 0;
  QTimer* time_listen_;
  int index = 0;
  bool listen_exist_ = false;
  static TouchKeyboardManager keyboard_manager_;
};
