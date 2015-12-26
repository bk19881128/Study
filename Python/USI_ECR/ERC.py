import math
import random
import xlrd
import xlwt
import sys
#print('log(e)=',math.log(math.e))
#name = eval(input('please input a num\n'))
#print('number is ',  name)
#test = {'1':'aaa','2':'bbb'}
#for x1,x2 in test.items():
#    print(x1,x2)
#list1 = [1,2,3,4,4,4,5,5,6]
#set1 = set(list1)
#set1.add(7)
#print(set1)

# cnt = 0#count the sum of result
# for i in range(1,5):
#     for j in range(1,5):
#         for k in range(1,5):
#             if i!=j and i!=k and j!=k:
#                 print(i*100+j*10+k)
#                 cnt += 1
# print(cnt)


# from datetime import datetime
# 
# style0 = xlwt.easyxf('font: name Times New Roman, color-index red, bold on',
#     num_format_str='#,##0.00')
# style1 = xlwt.easyxf(num_format_str='yy-mm-dd')
# 
# wb = xlwt.Workbook()
# ws = wb.add_sheet('A Test Sheet')
# 
# ws.write(0, 0, 1234.56, style0)
# ws.write(1, 0, datetime.now(), style1)
# ws.write(2, 0, 1)
# ws.write(2, 1, 1)
# ws.write(2, 2, xlwt.Formula("A3*B3"))
# 
# wb.save('example.xls')

def open_excel(file= 'file.xls'):
    try:
        data = xlrd.open_workbook(file)
        return data
    except :
        print('error open file')
        
def excel_table_byindex(file= 'file.xls',colnameindex=0,by_index=0):
    wb = xlwt.Workbook()
    ws = wb.add_sheet('Result Sheet')
    
    data = open_excel(file)
#     print('sucess open')
    table = data.sheets()[by_index]
    nrows = table.nrows 
#     print('nrows=',nrows)
    ncols = table.ncols 
#     print('ncols=',ncols)
    cell_value1 =  table.row_values(colnameindex)[1]
    print('cell_value1=',cell_value1) 
    cell_value_s1 = cell_value1.split(',') 
    print('cell_value_s1=',cell_value_s1)
    cell_value2 =  table.row_values(colnameindex+1)[1]
    print('cell_value2=',cell_value2) 
    cell_value_s2 = cell_value2.split(',') 
    print('cell_value_s2=',cell_value_s2)
    common_value = []
    output_value1 = []
    output_value2 = []
    for i in range(0,len(cell_value_s1)):
        if cell_value_s1[i] not in cell_value_s2:
            print(cell_value_s1[i],'is not in s2',)
            output_value1.append(cell_value_s1[i])
    print(output_value1)
    output_value_join1=",".join(output_value1)
    print(output_value_join1)
    
    for i in range(0,len(cell_value_s2)):
        if cell_value_s2[i] not in cell_value_s1:
            print(cell_value_s2[i],'is not in s1')
            output_value2.append(cell_value_s2[i])
    print(output_value2)
    output_value_join2 = ",".join(output_value2)
    print(output_value_join2)
    
    ws.write(0,0,len(output_value1))
    ws.write(0,1,output_value_join1)
    ws.write(1,0,len(output_value2))
    ws.write(1,1,output_value_join2)
    wb.save('result.xls')
#     return output_value
 
# def excel_write(input_value):
#     write_value = input_value
#     wb = xlwt.Workbook()
#     ws = wb.add_sheet('Result Sheet')
#     ws.write(2,1,write_value)
#     wb.save('result.xls')
    
    
#         for j in range(0,len(cell_value_s2)):
#             if cell_value_s1[i]!=cell_value_s2[j]:
#                 common_value.append(cell_value_s1(j)) 
#                 print('common_value=',common_value)   
                 
                
                        
#         print(cell_value_s1[i],end='==')
#     list =[]
#     for rownum in range(1,nrows):
# 
#         row = table.row_values(rownum)
#         if row:
#             app = {}
#             for i in range(len(colnames)):
#                 app[colnames[i]] = row[i] 
#             list.append(app)
#     return list
excel_table_byindex('example.xls',0,0)
# value_return = excel_table_byindex('example.xls',0,0)
# print('final return is ',value_return)
# excel_write(value_return)



