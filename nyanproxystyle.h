#ifndef NYANPROXYSTYLE_H
#define NYANPROXYSTYLE_H

#include <QPixmap>
#include <QProxyStyle>
#include <QSet>

class NyanProxyStyle : public QProxyStyle {
  Q_OBJECT

public:
  NyanProxyStyle(QStyle* base_style = 0);
  ~NyanProxyStyle();

  QSize sizeFromContents(ContentsType type, const QStyleOption* option,
                         const QSize& size, const QWidget* widget) const;

  void drawControl(ControlElement element, const QStyleOption* option,
                   QPainter* painter, const QWidget* widget) const;

  void polish(QWidget* widget);
  void polish(QPalette& pal) { return QProxyStyle::polish(pal); }
  void polish(QApplication* app) { return QProxyStyle::polish(app); }

protected:
  bool event(QEvent* e);
  bool eventFilter(QObject* o, QEvent* e);

private slots:
  void ProgressBarDestroyed(QObject* o);

private:
  static const int kHeight = 21;
  static const int kWidthCat = 34;
  static const int kWidthRainbow = 16;
  static const int kFrameCountCat = 5;
  static const int kFrameCountRainbow = 2;
  static const int kPadding = 2;
  static const int kRainbowOverlap = 13;
  static const int kAnimationTimeoutMs = 150;
  static const int kTextMarginOuter = 8;
  static const int kTextMarginInnerX = -1;
  static const int kTextMarginInnerY = 2;

private:
  inline QRect SourceRectCat() const {
    return QRect(0, kHeight * frame_cat_, kWidthCat, kHeight);
  }
  inline QRect SourceRectRainbow() const {
    return QRect(0, kHeight * frame_rainbow_, kWidthRainbow, kHeight);
  }

  void StartOrStopTimer();
  QPixmap TextBackground(int width, int height) const;

private:
  QStyle* cleanlooks_style_;
  QSet<QObject*> registered_progress_bars_;
  QSet<QObject*> visible_progress_bars_;

  QPixmap cat_;
  QPixmap rainbow_;

  int frame_cat_;
  int frame_rainbow_;

  int timer_id_;
};

#endif // NYANPROXYSTYLE_H
