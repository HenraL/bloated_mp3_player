import os
import sys

workdir = os.path.dirname(os.path.abspath(__file__))
tesseract_bin = os.path.join(workdir, "tesseract_local", "usr", "bin", "tesseract")
tessdata_dir = os.path.join(workdir, "tesseract_local", "usr", "share", "tesseract-ocr", "5", "tessdata")
lib_dir = os.path.join(workdir, "tesseract_local", "usr", "lib", "x86_64-linux-gnu")

os.environ["LD_LIBRARY_PATH"] = lib_dir + ":" + os.environ.get("LD_LIBRARY_PATH", "")
os.environ["TESSDATA_PREFIX"] = tessdata_dir

import pytesseract
pytesseract.pytesseract.tesseract_cmd = tesseract_bin

from PIL import Image

for img_name in ["IMG_20260605_164445.jpg", "IMG_20260605_164448.jpg"]:
    img_path = os.path.join(workdir, img_name)
    print(f"\n{'='*60}")
    print(f"FILE: {img_name}")
    print(f"{'='*60}")
    try:
        text = pytesseract.image_to_string(Image.open(img_path))
        print(text if text.strip() else "[No text detected]")
    except Exception as e:
        print(f"Error: {e}")

