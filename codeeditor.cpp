#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <QEvent>

// 覆盖在编辑器视口左侧空白处的小部件，只负责转发绘制请求。
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), m_codeEditor(editor) {}

    QSize sizeHint() const override
    {
        return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_codeEditor;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditor::highlightCurrentLine);

    // 编辑器外观
    setFrameShape(QFrame::NoFrame);
    setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() const
{
    if (!m_lineNumbersVisible)
        return 0;

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    // 左右各留 4 像素，再按数字宽度计算
    return 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                        lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::changeEvent(QEvent *event)
{
    QPlainTextEdit::changeEvent(event);
    if (event->type() == QEvent::FontChange)
        updateLineNumberAreaWidth(0);
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    if (m_lineNumbersVisible == visible)
        return;

    m_lineNumbersVisible = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(0);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                        lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::setEditorBackgroundColor(const QColor &color)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, color);
    setPalette(pal);
}

void CodeEditor::setCurrentLineColor(const QColor &color)
{
    m_currentLineColor = color;
    highlightCurrentLine();
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> selections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(m_currentLineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }

    setExtraSelections(selections);
    m_lineNumberArea->update();   // 行号区同步刷新当前行底色
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!m_lineNumbersVisible)
        return;

    QPainter painter(m_lineNumberArea);

    // 先铺行号区底色
    painter.fillRect(event->rect(), m_gutterColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const int currentBlock = textCursor().blockNumber();
    const int numberWidth = m_lineNumberArea->width() - 6;

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            // 当前行在行号区也着色，形成一条连续的黄色横带
            if (blockNumber == currentBlock) {
                painter.fillRect(0, top, m_lineNumberArea->width(),
                                 fontMetrics().height() + 2, m_currentLineColor);
            }

            painter.setPen(QColor(0x8A, 0x8A, 0x8A));
            painter.drawText(0, top, numberWidth, fontMetrics().height(),
                             Qt::AlignRight, QString::number(blockNumber + 1));
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
