import numpy as np

n = 15
m = 4

# 検査符号のビット数
parity_bits = n - m

# 検査行列Hを生成
H_left = np.eye(parity_bits)
H_right = np.eye(parity_bits, m)
H = np.concatenate((H_left, H_right), axis=1)

# 生成行列Gを計算
P = np.transpose(np.eye(parity_bits))
G = np.concatenate((P, np.mod(np.dot(H[:, :parity_bits], P), 2)), axis=1)

print("生成行列 G:\n", G.astype(int))
print("検査行列 H:\n", H.astype(int))
