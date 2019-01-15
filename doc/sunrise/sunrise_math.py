import sys

def r(x):
    #if x<29:
    #    return ((246-0)/(28-0))*(x-0)+0
    #elif x<100:
    #    return 255
    #elif x<279:
    #    return ((150-254)/(279-100))*(x-100)+254
    #else:
    #    return ((128-149)/(299-279))*(x-279)+149
    if x<29:
        return 8.785713*x
    elif x<100:
        return 255
    elif x<279:
        return -0.581006*(x-100)+254
    else:
        return -1.05*(x-279)+149

def g(x):
    #if x<31:
    #    return 0
    #elif x<99:
    #    return ((147-2)/(98-31))*(x-31)+2
    #elif x<279:
    #    return 150
    #else:
    #    return ((128-149)/(299-279))*(x-279)+149
    if x<31:
        return 0
    elif x<99:
        return 2.164180*(x-31)+2
    elif x<279:
        return 150
    else:
        return -1.05*(x-279)+149

def b(x):
    #if x<100:
    #    return 0
    #elif x<170:
    #    return ((90-1)/(169-100))*(x-100)+1
    #elif x<290:
    #    return ((244-92)/(289-170))*(x-170)+92
    #else:
    #    return ((255-245)/(299-290))*(x-290)+245
    if x<100:
        return 0
    elif x<170:
        return 1.289854*(x-100)+1
    elif x<290:
        return 1.277311*(x-170)+92
    else:
        return 1.111110*(x-290)+245

for x in range(300):
    vr, vg, vb = r(x), g(x), b(x)
    print(x, round(vr, 4), round(vg, 4), round(vb, 4), file=sys.stderr)
    print(f"{round(vr)}, {round(vg)}, {round(vb)}")
