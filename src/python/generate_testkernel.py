import sys

testcase = sys.argv[1]

with open('../c/testkerneltemplate.c', 'r') as f:
    template_string = f.read()
    template_string = template_string.replace('testcase', 'testcase' + testcase)

with open('../c/testkernel.c', 'w') as f:
    f.write(template_string)
