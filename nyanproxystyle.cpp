#include "nyanproxystyle.h"

#include <QCleanlooksStyle>
#include <QEvent>
#include <QPainter>
#include <QPixmapCache>
#include <QProgressBar>
#include <QStyleOption>
#include <QtDebug>


// Exported by QtGui
void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);


NyanProxyStyle::NyanProxyStyle(QStyle* base_style)
  : QProxyStyle(base_style),
    cleanlooks_style_(new QCleanlooksStyle),
    cat_(":/nyancat.png"),
    rainbow_(":/rainbow.png"),
    frame_cat_(0),
    frame_rainbow_(0),
    timer_id_(-1) {
}

NyanProxyStyle::~NyanProxyStyle() {
  delete cleanlooks_style_;
}

QSize NyanProxyStyle::sizeFromContents(ContentsType type,
                                       const QStyleOption* option,
                                       const QSize& size,
                                       const QWidget* widget) const {
  switch (type) {
  case CT_ProgressBar:
    return QSize(size.width(), kHeight + kPadding*2);
  default:
    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }
}

void NyanProxyStyle::drawControl(ControlElement element,
                                 const QStyleOption* option,
                                 QPainter* painter,
                                 const QWidget* widget) const {
  switch (element) {
  case CE_ProgressBarGroove: {
    cleanlooks_style_->drawControl(element, option, painter, widget);
    break;
  }
  case CE_ProgressBarContents: {
    const QStyleOptionProgressBar* opt =
        qstyleoption_cast<const QStyleOptionProgressBar*>(option);

    // Calculate the position of nyancat
    const int total_extent = option->rect.width() - kPadding*2 - kWidthCat;
    const QRect dest_rect_cat(opt->rect.left() + kPadding +
          qreal(opt->progress - opt->minimum) /
          qreal(opt->maximum  - opt->minimum) * total_extent,
          opt->rect.top() + kPadding, kWidthCat, kHeight);
    const int rainbow_extent = dest_rect_cat.x() - kPadding - opt->rect.left()
        + kRainbowOverlap;

    // Draw the rainbow
    for (int dest_x = 0 ; dest_x < rainbow_extent ; dest_x += kWidthRainbow) {
      const int width = (kWidthRainbow < rainbow_extent - dest_x) ?
            kWidthRainbow : rainbow_extent - dest_x;
      const QRect dest_rect_rainbow(
            opt->rect.left() + kPadding + dest_x,
            opt->rect.top() + kPadding,
            width, kHeight);

      painter->drawPixmap(dest_rect_rainbow, rainbow_, SourceRectRainbow());
    }

    // Draw nyancat
    painter->drawPixmap(dest_rect_cat, cat_, SourceRectCat());
    break;
  }
  case CE_ProgressBarLabel: {
    const QStyleOptionProgressBar* opt =
        qstyleoption_cast<const QStyleOptionProgressBar*>(option);

    const int text_width = option->fontMetrics.width(opt->text);
    const int text_height = option->fontMetrics.height();

    // Draw the background centered in the bar.
    QPixmap background(TextBackground(text_width, text_height));
    QRect background_rect(background.rect());
    background_rect.moveCenter(opt->rect.center());

    painter->drawPixmap(background_rect, background);

    // Draw the text on top.
    painter->drawText(background_rect, Qt::AlignHCenter | Qt::AlignVCenter, opt->text);
    break;
  }
  default:
    QProxyStyle::drawControl(element, option, painter, widget);
  }
}

void NyanProxyStyle::polish(QWidget* widget) {
  if (qobject_cast<QProgressBar*>(widget)) {
    connect(widget, SIGNAL(destroyed(QObject*)), SLOT(ProgressBarDestroyed(QObject*)));
    widget->installEventFilter(this);
    registered_progress_bars_ << widget;
  }

  QProxyStyle::polish(widget);
}

bool NyanProxyStyle::eventFilter(QObject* o, QEvent* e) {
  if (registered_progress_bars_.contains(o)) {
    switch (e->type()) {
    case QEvent::Hide:
      visible_progress_bars_.remove(o);
      StartOrStopTimer();
      break;
    case QEvent::Show:
      visible_progress_bars_.insert(o);
      StartOrStopTimer();
      break;
    default:
      break;
    }
  }

  return QProxyStyle::eventFilter(o, e);
}

void NyanProxyStyle::StartOrStopTimer() {
  if (visible_progress_bars_.isEmpty() && timer_id_ != -1) {
    killTimer(timer_id_);
    timer_id_ = -1;
  } else if (!visible_progress_bars_.isEmpty() && timer_id_ == -1) {
    timer_id_ = startTimer(kAnimationTimeoutMs);
  }
}

bool NyanProxyStyle::event(QEvent* e) {
  if (e->type() != QEvent::Timer || static_cast<QTimerEvent*>(e)->timerId() != timer_id_) {
    return QProxyStyle::event(e);
  }

  frame_cat_     = (frame_cat_     + 1) % kFrameCountCat;
  frame_rainbow_ = (frame_rainbow_ + 1) % kFrameCountRainbow;

  foreach (QObject* o, visible_progress_bars_.values()) {
    static_cast<QWidget*>(o)->update();
  }

  e->accept();
  return true;
}

void NyanProxyStyle::ProgressBarDestroyed(QObject* o) {
  visible_progress_bars_.remove(o);
  registered_progress_bars_.remove(o);
}

QPixmap NyanProxyStyle::TextBackground(int width, int height) const {
  const QString key = QString("NyanProxyStyle::TextBackground/%1/%2").arg(width).arg(height);

  QPixmap ret;
  if (QPixmapCache::find(key, &ret)) {
    qDebug() << "cache hit" << key;
    return ret;
  }

  const QSize size(width + kTextMarginOuter*2 - kTextMarginInnerX*2,
                   height + kTextMarginOuter*2 - kTextMarginInnerY*2);
  const QColor color(255, 255, 255, 200);

  QImage temp_image(size, QImage::Format_ARGB32_Premultiplied);
  temp_image.fill(Qt::transparent);

  // Draw a white background for the text.
  QPainter painter(&temp_image);
  painter.setPen(color);
  painter.setBrush(color);
  painter.drawRoundedRect(kTextMarginOuter,
                          kTextMarginOuter,
                          width - kTextMarginInnerX*2,
                          height - kTextMarginInnerY*2,
                          5, 5);
  painter.end();

  // Blur the background onto the final image
  ret = QPixmap(size);
  ret.fill(Qt::transparent);

  painter.begin(&ret);
  qt_blurImage(&painter, temp_image, 10.0, true, false);
  painter.end();

  qDebug() << "cache miss" << key;
  QPixmapCache::insert(key, ret);
  return ret;
}
