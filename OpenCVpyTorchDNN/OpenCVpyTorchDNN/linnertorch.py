'''
Author: Vaccae
Date: 2022-11-01 11:11:05
LastEditors: Vaccae
LastEditTime: 2022-11-07 14:16:31
FilePath: \test\base01\linnertorch.py
Description: 

Copyright (c) 2022 by Vaccae 3657447@qq.com, All Rights Reserved. 
'''

import torch
import numpy as np

x_data = torch.Tensor([[1.0], [2.0], [3.0]])
y_data =  torch.Tensor([[2.0], [4.0], [6.0]])

print(x_data)
print(y_data)


class LinearModel(torch.nn.Module):
    def __init__(self):
        super(LinearModel, self).__init__()
        self.linear = torch.nn.Linear(1, 1)

    def forward(self, x):
        y_pred = self.linear(x)
        return y_pred

model = LinearModel()

##损失函数
criterion = torch.nn.MSELoss(size_average=False)
##优化器
optimizer = torch.optim.SGD(model.parameters(), lr=0.02)

##步骤
## 1.求Y hat  2. loss   3.backward   4.step
for epoch in range(1000):
    y_pred = model(x_data)
    loss = criterion(y_pred, y_data)
    print(epoch, loss)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

#打印权重值
print('w = ', model.linear.weight.item())
print('b = ', model.linear.bias.item())

#测试 Model
x_test = torch.Tensor([[4.0]])
y_test = model(x_test)
print('y_pred =  ', y_test.data)

x_test = torch.Tensor([[8.0]])
y_test = model(x_test)
print('y_pred =  ', y_test.data)

x_test = torch.Tensor([[10.0]])
y_test = model(x_test)
print('y_pred =  ', y_test.data)

x_test = torch.Tensor([[15.0]])
y_test = model(x_test)
print('y_pred =  ', y_test.data)


##使用eval()将模型设置为“推理模式”
model.eval()

##导出模型为onnx
dummy_input = torch.randn(1,1)
##定义输出输出的参数名
input_name = ["input"]
output_name = ["output"]

onnx_name = 'test.onnx'

torch.onnx.export(
    model,
    dummy_input,
    onnx_name,
    verbose=True,
    input_names=input_name,
    output_names=output_name
)

