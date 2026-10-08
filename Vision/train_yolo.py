from ultralytics import YOLO

model = YOLO("yolo26n-pose")

model.train(
    data="data.yaml",
    epochs=100,
    fliplr=0.0,
    save_period=10,
    batch=20,
)