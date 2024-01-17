import openpyxl

# 从txt文件中读取数据
with open("tt2.txt", "r") as file:
    lines = file.readlines()

# 解析txt文件中的矩阵数据
matrix_data = [list(map(int, line.split())) for line in lines]

# 创建一个新的Excel工作簿和工作表
workbook = openpyxl.Workbook()
sheet = workbook.active

# 将矩阵数据写入9x9的表格
for i in range(9):
    for j in range(9):
        sheet.cell(row=i+1, column=j+1, value=matrix_data[i][j])

# 保存工作簿为Excel文件
workbook.save("output3.xlsx")

print("Excel文件已生成。")
