import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision import datasets, models, io, transforms 
from copy import deepcopy
import onnx
import onnxruntime
import cv2
from onnx.onnx_pb import ModelProto
from onnxruntime import InferenceSession, SessionOptions
from onnxruntime.quantization import QuantizationMode
from onnxruntime.quantization.onnx_quantizer import ONNXQuantizer
from onnxruntime.quantization.registry import IntegerOpsRegistry

aug_transform = transforms.Compose([
            transforms.ToPILImage(),
            transforms.Resize((256, 256)),
            transforms.ToTensor(),
            transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225]) #DN
        ])

class Vgg19_v4(nn.Module):
    def __init__(self):
        super(Vgg19_v4, self).__init__()

        vgg19 = models.vgg19(weights=None) #(pretrained=True)
        for name, param in vgg19.named_parameters():
          if param.requires_grad and 'features' in name:
            if 'features.34' in name:
              param.requires_grad = True
            else:
              param.requires_grad = False
        vgg19.classifier[-1] = nn.Linear(vgg19.classifier[-1].in_features, 16)
        
        self.vgg = vgg19
    
    def forward(self, x):
        output = self.vgg(x)
        #output = torch.sigmoid(output)
        return output

class model(nn.Module):
    def __init__(self, checkpoint_path, quantization=False):
        super(model, self).__init__()
        
        checkpoint = torch.load(checkpoint_path)
        vgg = Vgg19_v4()
        vgg.load_state_dict(checkpoint['model_state_dict'])

        self.backbone = vgg.vgg.features
        self.custom_pool = nn.AvgPool2d(kernel_size=2, stride=1)
        self.classifier = vgg.vgg.classifier


        self.model = nn.ModuleList([self.backbone, self.custom_pool, self.classifier])
    
    def forward(self, x):
        for idx,layer in enumerate(self.model):
            x = layer(x)
            if idx == 1:
                x = torch.flatten(x, start_dim=1)
        return F.softmax(x, dim=1)

image = cv2.imread('/workspace/omkar_projects/XAI/without.jpeg')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
# image = cv2.resize(image, (256,256))
# image = image.astype('float32') / 255.0
# image = image.transpose(2,0,1)[None,:,:,:]
# image = torch.from_numpy(image)
# image = image.unsqueeze(0)

image = aug_transform(image).unsqueeze(0)

# print(image)

vgg2 = model('vggftlclnone_adam_0.01_best_model.pth')
vgg2.eval()

# print(vgg2(image))

#onnxruntime model inference
# sess_options = SessionOptions()
# sess_options.intra_op_num_threads = 1
# sess_options.execution_mode = onnxruntime.ExecutionMode.ORT_SEQUENTIAL
# sess_options.graph_optimization_level = onnxruntime.GraphOptimizationLevel.ORT_ENABLE_ALL
# sess_options.optimized_model_filepath = "vgg19.onnx"

# sess = InferenceSession("vgg19.onnx", providers=['CUDAExecutionProvider'])
# input_name = sess.get_inputs()[0].name
# label_name = sess.get_outputs()[0].name
# print(input_name, label_name)
# pred_onnx = sess.run([label_name], {input_name: image})[0]
# print(pred_onnx)



torch.onnx.export(vgg2, image, "vgg19.onnx", verbose=True, input_names=['input'], output_names=['output'], opset_version=11)