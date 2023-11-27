import requests
import base64

# onnx_path = "/open_explorer/workspace/projects/flowengine/tests/data/models/best.onnx"
onnx_path = "/open_explorer/workspace/projects/flowengine/scripts/x3_convert/resnet-10_ce_face_occlusion_231117_128x128_99.958.onnx"
with open(onnx_path, mode="rb") as rbf:
    data = rbf.read()


# hyper_params = """{
#     "model_parameter":
#         {"img_size": 640}}"""

hyper_params = """{
    "model_parameter": {
        "img_size": [
            128,
            128
        ]
    }
}"""

url = 'http://localhost:9888/convert_x3'
post_data = {
    'idx': 0,
    'onnx_params': base64.b64encode(data).decode(encoding="utf-8"),
    "hyper_params": hyper_params
}
x = requests.post(url, json=post_data)
