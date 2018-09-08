from datetime import datetime
import random

def initialize():
    # initialize once
    random.seed(datetime.now())
    random.random()
    return

def match():
    # one match only
    ari = list()
    ahi = list()
    hri = list()
    hhi = list()

    for i in range(9):
        ar, ah = side()
        ari.append(ar)
        ahi.append(ah)

    for i in range(9):
        print ari[i],
    print
    for i in range(9):
        print ahi[i],
    
    return

def side():
    r = int (random.random() * 3.)
    h = int (random.random() * 3.) + r
    return r, h

initialize()
match()

