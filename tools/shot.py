# -*- coding: utf-8 -*-
"""按窗口标题截取窗口区域，保存为 PNG（用于实验报告截图）。"""
import ctypes
import ctypes.wintypes as wt
import os
import sys
from PIL import ImageGrab

try:
    ctypes.windll.shcore.SetProcessDpiAwareness(2)
except Exception:
    ctypes.windll.user32.SetProcessDPIAware()

user32 = ctypes.windll.user32

OUT_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "shots")
os.makedirs(OUT_DIR, exist_ok=True)


def find_window(substr, exact=False):
    found = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(hwnd, _):
        if not user32.IsWindowVisible(hwnd):
            return True
        n = user32.GetWindowTextLengthW(hwnd)
        if n == 0:
            return True
        buf = ctypes.create_unicode_buffer(n + 1)
        user32.GetWindowTextW(hwnd, buf, n + 1)
        title = buf.value
        if (title == substr) if exact else (substr in title):
            found.append((hwnd, title))
        return True

    user32.EnumWindows(cb, 0)
    return found


def main():
    if len(sys.argv) < 3:
        print("usage: shot.py <title-substring> <out-name>")
        return 1

    title, name = sys.argv[1], sys.argv[2]
    wins = find_window(title)
    if not wins:
        print("window not found:", title)
        return 2

    hwnd, real_title = wins[0]
    rect = wt.RECT()
    # DWMWA_EXTENDED_FRAME_BOUNDS(9)：去掉窗口阴影/不可见的调整边框
    ok = ctypes.windll.dwmapi.DwmGetWindowAttribute(
        wt.HWND(hwnd), 9, ctypes.byref(rect), ctypes.sizeof(rect))
    if ok != 0:
        user32.GetWindowRect(hwnd, ctypes.byref(rect))
    l, t, r, b = rect.left, rect.top, rect.right, rect.bottom
    print("window:", real_title, (l, t, r, b))

    img = ImageGrab.grab(bbox=(l, t, r, b), all_screens=True)
    path = os.path.join(OUT_DIR, name if name.endswith(".png") else name + ".png")
    img.save(path)
    print("saved:", path, img.size)
    return 0


if __name__ == "__main__":
    sys.exit(main())
