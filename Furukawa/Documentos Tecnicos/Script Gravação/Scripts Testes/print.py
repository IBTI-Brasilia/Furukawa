def printLog(*args, **kwargs):
    print(*args, **kwargs)
    with open('Log.txt','a') as file:
        print(*args, **kwargs, file=file)