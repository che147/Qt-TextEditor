#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QColor>

class QWidget;

// 继承 QPlainTextEdit，在左侧绘制行号，并高亮当前行。
// 该类通过 Qt Designer 的“提升为（promote）”功能替换原来的 QPlainTextEdit。
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // 行号区域宽度（由当前行数位数决定）
    int lineNumberAreaWidth() const;
    // 由 LineNumberArea 在 paintEvent 中调用
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    void setLineNumbersVisible(bool visible);
    bool lineNumbersVisible() const { return m_lineNumbersVisible; }

    // 编辑器背景色 / 当前行高亮色
    void setEditorBackgroundColor(const QColor &color);
    void setCurrentLineColor(const QColor &color);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    QWidget *m_lineNumberArea = nullptr;
    bool m_lineNumbersVisible = true;
    QColor m_gutterColor = QColor(0xF7, 0xF7, 0xF7);
    QColor m_currentLineColor = QColor(0xFF, 0xFF, 0xCC);
};

#endif // CODEEDITOR_H
