import cv2
import os
import argparse

# Set up argument parsing
parser = argparse.ArgumentParser(description='Take photos using the webcam.')
parser.add_argument('name', type=str, help='Name to be used in the photo filenames')
args = parser.parse_args()

name = args.name

# Create the dataset directory if it doesn't exist
dataset_dir = os.path.join('dataset', name)
os.makedirs(dataset_dir, exist_ok=True)

cam = cv2.VideoCapture(0)

cv2.namedWindow("press space to take a photo", cv2.WINDOW_NORMAL)
cv2.resizeWindow("press space to take a photo", 500, 300)

img_counter = 0

while True:
    ret, frame = cam.read()
    if not ret:
        print("failed to grab frame")
        break
    cv2.imshow("press space to take a photo", frame)

    k = cv2.waitKey(1)
    if k % 256 == 27:
        # ESC pressed
        print("Escape hit, closing...")
        break
    elif k % 256 == 32:
        # SPACE pressed
        img_name = os.path.join(dataset_dir, "image_{}.jpg".format(img_counter))
        cv2.imwrite(img_name, frame)
        print("{} written!".format(img_name))
        img_counter += 1

cam.release()
cv2.destroyAllWindows()
