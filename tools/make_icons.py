# -*- coding: utf-8 -*-
"""生成工具栏 / 菜单用的 24x24 图标（4 倍绘制后缩放，带抗锯齿）。"""
import os
from PIL import Image, ImageDraw, ImageFont

S = 4          # 超采样倍数
SIZE = 24
OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "icons")
os.makedirs(OUT, exist_ok=True)

FONT_BOLD = r"C:\Windows\Fonts\arialbd.ttf"
FONT_SERIF = r"C:\Windows\Fonts\timesbd.ttf"


def canvas():
    img = Image.new("RGBA", (SIZE * S, SIZE * S), (0, 0, 0, 0))
    return img, ImageDraw.Draw(img)


def save(img, name):
    img.resize((SIZE, SIZE), Image.LANCZOS).save(os.path.join(OUT, name), "PNG")


def page(d, x, y, w, h, fill, outline, fold=8):
    """带折角的纸"""
    d.polygon([(x, y), (x + w - fold, y), (x + w, y + fold), (x + w, y + h), (x, y + h)],
              fill=fill, outline=outline)


def load_font(path, size):
    return ImageFont.truetype(path, size)


def text_center(d, box, s, font, fill):
    l, t, r, b = d.textbbox((0, 0), s, font=font)
    w, h = r - l, b - t
    x = box[0] + (box[2] - box[0] - w) / 2 - l
    y = box[1] + (box[3] - box[1] - h) / 2 - t
    d.text((x, y), s, font=font, fill=fill)


def arrow_head(d, tip, direction, size, fill):
    x, y = tip
    if direction == "left":
        d.polygon([(x, y), (x + size, y - size * 0.7), (x + size, y + size * 0.7)], fill=fill)
    elif direction == "right":
        d.polygon([(x, y), (x - size, y - size * 0.7), (x - size, y + size * 0.7)], fill=fill)


def icon_new():
    img, d = canvas()
    page(d, 20, 12, 56, 72, "#FFFFFF", "#5A5A5A", 16)
    d.line([(52, 12), (52, 30), (70, 30)], fill="#7A7A7A", width=3)
    save(img, "new.png")


def icon_open():
    img, d = canvas()
    d.rounded_rectangle([8, 22, 88, 82], 6, fill="#E2A03C", outline="#A9741F", width=3)
    d.polygon([(10, 26), (38, 26), (48, 40), (12, 40)], fill="#F0C05A", outline="#A9741F")
    d.rounded_rectangle([8, 38, 88, 82], 6, fill="#F7C96B", outline="#A9741F", width=3)
    save(img, "open.png")


def icon_save():
    img, d = canvas()
    d.rounded_rectangle([12, 10, 84, 86], 6, fill="#4A82C4", outline="#2C5A96", width=3)
    d.rectangle([26, 10, 70, 38], fill="#EFF4FA", outline="#2C5A96", width=3)
    d.rectangle([56, 14, 66, 34], fill="#2C5A96")
    d.rectangle([24, 54, 72, 82], fill="#FFFFFF", outline="#2C5A96", width=3)
    save(img, "save.png")


def icon_saveas():
    img, d = canvas()
    d.rounded_rectangle([8, 8, 66, 74], 6, fill="#4A82C4", outline="#2C5A96", width=3)
    d.rectangle([20, 8, 54, 30], fill="#EFF4FA", outline="#2C5A96", width=3)
    d.rectangle([18, 44, 56, 68], fill="#FFFFFF", outline="#2C5A96", width=3)
    # 铅笔
    d.polygon([(74, 82), (86, 70), (92, 76), (80, 88)], fill="#F0A32C", outline="#A96B14")
    d.polygon([(72, 90), (74, 82), (80, 88)], fill="#F5D9A8", outline="#A96B14")
    save(img, "saveas.png")


def icon_undo(color="#3F9B44"):
    img, d = canvas()
    f = ImageFont.truetype(r"C:\Windows\Fonts\seguisym.ttf", 88)
    d.text((SIZE * S / 2, SIZE * S / 2), "\u21b6", font=f, fill=color, anchor="mm")
    save(img, "undo.png")


def icon_redo(color="#8FA8C4"):
    img, d = canvas()
    f = ImageFont.truetype(r"C:\Windows\Fonts\seguisym.ttf", 88)
    d.text((SIZE * S / 2, SIZE * S / 2), "\u21b7", font=f, fill=color, anchor="mm")
    save(img, "redo.png")


def icon_cut():
    img, d = canvas()
    d.line([(20, 14), (62, 70)], fill="#8C8C8C", width=6)
    d.line([(76, 14), (34, 70)], fill="#B0B0B0", width=6)
    d.line([(20, 14), (62, 70)], fill="#8C8C8C", width=3)
    d.ellipse([24, 66, 42, 84], outline="#C0392B", width=6)
    d.ellipse([54, 66, 72, 84], outline="#C0392B", width=6)
    save(img, "cut.png")


def icon_copy():
    img, d = canvas()
    page(d, 14, 26, 48, 60, "#E6E6E6", "#9A9A9A", 12)
    page(d, 26, 12, 50, 62, "#FFFFFF", "#5A5A5A", 12)
    save(img, "copy.png")


def icon_paste():
    img, d = canvas()
    d.rounded_rectangle([14, 16, 80, 88], 6, fill="#C8A36A", outline="#8A6A34", width=3)
    d.rectangle([26, 10, 68, 30], fill="#EFE2CC", outline="#8A6A34", width=3)
    d.rectangle([24, 34, 70, 80], fill="#FFFFFF", outline="#8A6A34", width=2)
    for y in (44, 54, 64):
        d.line([(32, y), (62, y)], fill="#B0B0B0", width=3)
    save(img, "paste.png")


def icon_font():
    img, d = canvas()
    f = load_font(FONT_SERIF, 74)
    text_center(d, (6, 8, 92, 90), "A", f, "#2F6FB3")
    save(img, "font.png")


def icon_fontcolor():
    img, d = canvas()
    f = load_font(FONT_SERIF, 64)
    text_center(d, (6, 2, 92, 70), "A", f, "#333333")
    d.rectangle([10, 78, 86, 90], fill="#D03A2F")
    save(img, "fontcolor.png")


def icon_fontbg():
    img, d = canvas()
    f = load_font(FONT_SERIF, 64)
    d.rectangle([6, 8, 90, 76], fill="#FFF07A")
    text_center(d, (6, 2, 92, 76), "A", f, "#333333")
    d.rectangle([10, 80, 86, 90], fill="#E8C51E")
    save(img, "fontbg.png")


def icon_editbg():
    img, d = canvas()
    d.rounded_rectangle([16, 30, 76, 86], 8, fill="#5AA0D8", outline="#2C6A9B", width=3)
    d.arc([34, 14, 74, 54], 200, 340, fill="#2C6A9B", width=6)
    d.ellipse([16, 52, 30, 66], fill="#7FC0E8", outline="#2C6A9B", width=2)
    save(img, "editbg.png")


def icon_linenumber():
    img, d = canvas()
    d.rectangle([6, 10, 26, 86], fill="#F0F0F0", outline="#B0B0B0")
    f = load_font(FONT_BOLD, 18)
    for i, y in enumerate((14, 40, 66), start=1):
        text_center(d, (6, y, 26, y + 18), str(i), f, "#666666")
    for y in (24, 46, 68):
        d.line([(32, y), (88, y)], fill="#3A3A3A", width=4)
    save(img, "linenumber.png")


def icon_wrap():
    img, d = canvas()
    for y in (16, 34):
        d.line([(12, y), (84, y)], fill="#7A7A7A", width=5)
    d.line([(12, 52), (58, 52)], fill="#7A7A7A", width=5)
    d.arc([40, 44, 76, 72], 180, 360, fill="#3C7EBF", width=6)
    arrow_head(d, (40, 66), "left", 14, "#3C7EBF")
    d.line([(12, 76), (84, 76)], fill="#7A7A7A", width=5)
    save(img, "wrap.png")


def icon_find():
    img, d = canvas()
    d.ellipse([10, 10, 62, 62], outline="#3C7EBF", width=8)
    d.line([(58, 58), (84, 84)], fill="#3C7EBF", width=10)
    save(img, "find.png")


def icon_replace():
    img, d = canvas()
    d.ellipse([8, 8, 54, 54], outline="#3C7EBF", width=7)
    d.line([(50, 50), (72, 72)], fill="#3C7EBF", width=9)
    d.polygon([(74, 84), (88, 70), (94, 76), (80, 90)], fill="#F0A32C", outline="#A96B14")
    save(img, "replace.png")


def icon_about():
    img, d = canvas()
    d.ellipse([8, 8, 88, 88], fill="#3C7EBF", outline="#2C5A96", width=2)
    f = load_font(FONT_BOLD, 52)
    text_center(d, (8, 12, 88, 78), "i", f, "#FFFFFF")
    save(img, "about.png")


def icon_app():
    img, d = canvas()
    page(d, 6, 4, 76, 92, "#FFFFFF", "#5A5A5A", 16)
    for y in (30, 44, 58, 72):
        d.line([(18, y), (62, y)], fill="#B0B0B0", width=4)
    d.polygon([(56, 88), (92, 52), (100, 60), (64, 96)], fill="#EF9A2E", outline="#A96B14")
    d.polygon([(52, 98), (56, 88), (64, 96)], fill="#F7DDB0", outline="#A96B14")
    img.resize((64, 64), Image.LANCZOS).save(os.path.join(OUT, "app.png"), "PNG")
    ico = img.resize((256, 256), Image.LANCZOS)
    ico.save(os.path.join(OUT, "app.ico"), sizes=[(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)])


for fn in (icon_new, icon_open, icon_save, icon_saveas, icon_undo, icon_redo, icon_cut,
           icon_copy, icon_paste, icon_font, icon_fontcolor, icon_fontbg, icon_editbg,
           icon_linenumber, icon_wrap, icon_find, icon_replace, icon_about, icon_app):
    fn()
print("icons ->", OUT)
