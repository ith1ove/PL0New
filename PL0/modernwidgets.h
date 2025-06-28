// 高级美化示例 - 可选实现

#ifndef MODERNWIDGETS_H
#define MODERNWIDGETS_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QPainter>
#include <QStyleOption>

// 现代化按钮类 - 带动画效果
class ModernButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal glowRadius READ glowRadius WRITE setGlowRadius)
    
public:
    explicit ModernButton(const QString &text, QWidget *parent = nullptr);
    
    qreal glowRadius() const { return m_glowRadius; }
    void setGlowRadius(qreal radius) { 
        m_glowRadius = radius; 
        update(); 
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPropertyAnimation *m_hoverAnimation;
    QPropertyAnimation *m_pressAnimation;
    qreal m_glowRadius;
    bool m_isHovered;
    bool m_isPressed;
};

// 现代化文本编辑器 - 带语法高亮
class ModernTextEdit : public QTextEdit {
    Q_OBJECT
    
public:
    explicit ModernTextEdit(QWidget *parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    void setupSyntaxHighlighter();
};

// 渐变背景窗口
class GradientWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit GradientWindow(QWidget *parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QLinearGradient m_backgroundGradient;
};

#endif // MODERNWIDGETS_H
