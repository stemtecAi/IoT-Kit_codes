import cv2
print("OpenCV imported successfully")
import torch
print("Pytorch imported successfully")
print(torch.__version__)
import time
from torchvision import transforms, models
print("Models imported successfully")
import numpy as np
print("numpy imported successfully")

# Load MobileNetV2 model for object detection
net = models.quantization.mobilenet_v2(pretrained=True, quantize=True)
net = torch.jit.script(net)
net.eval()

# Open camera
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 224)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 224)
cap.set(cv2.CAP_PROP_FPS, 36)

# Preprocessing transformations
preprocess = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
])

# Load ImageNet class labels
with open("imagenet_classes.txt") as f:
    classes = [line.strip() for line in f.readlines()]

# Create a window to display the camera feed
cv2.namedWindow("Object Detection", cv2.WINDOW_NORMAL)

# Main loop for object detection and camera preview
with torch.no_grad():
    while True:
        # Read frame from camera
        ret, frame = cap.read()
        if not ret:
            raise RuntimeError("Failed to read frame")

        # Convert frame from BGR to RGB
        rgb_frame = frame[:, :, [2, 1, 0]]

        # Preprocess frame
        input_tensor = preprocess(rgb_frame)
        input_batch = input_tensor.unsqueeze(0)

        # Run object detection model
        output = net(input_batch)

        # Get predicted class and bounding box coordinates
        _, predicted = torch.max(output, 1)
        predicted_class = classes[predicted.item()]
        predicted_confidence = torch.softmax(output, 1)[0][predicted.item()].item()

        # Display predicted class and confidence
        cv2.putText(frame, f"{predicted_class}: {predicted_confidence:.2f}", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)

        # Display camera feed with bounding box around the detected object
        cv2.imshow("Object Detection", frame)

        # Exit on ESC key
        key = cv2.waitKey(1)
        if key == 27:  # ESC
            break

# Release resources
cap.release()
cv2.destroyAllWindows()
