import os

import runPlot
import evPlot
import pandas as pd
import numpy as np
import plotly.graph_objects as go

def openFile(file):
    data = pd.DataFrame(np.loadtxt(file))
    return data

def ScanDir(dirPath) -> str: 
    # Get the list of all files and directories 
    # in current directory 
    l = []
    dirContent = os.listdir(dirPath) 
    for item in dirContent: 
        # Get the item's full path 
        itemPath = os.path.join(dirPath, item) 
        # If item is a directory then recursively 
        # scan that directory 
        if os.path.isdir(itemPath): 
            l = l+ScanDir(itemPath) 
        else: 
            # If item is a file then print its name 
            l.append(itemPath)
    return l

def FileSyncDirectoryCheck(sourceFile: str, sourceRoot: str, targetRoot: str) -> bool:
    sourceExtension = ".txt"
    targetExtension = ".png"
    file = sourceFile[len(sourceRoot):-len(sourceExtension)]
    targetFile = targetRoot + file + targetExtension
    f = os.path.splitext(str(targetFile).replace("predator_",""))
    os.makedirs(f[0], exist_ok=True)
    # if not os.path.exists(f[0]):
    #     return False
    return False
    return True

# Driver Code 
if __name__ == "__main__": 
    # Path of the directory
    evPlot.SKIP_IF_EXISTS = True 
    runPlot.SKIP_IF_EXISTS = evPlot.SKIP_IF_EXISTS 
    runDirPath = './build/plots/'
    runPlotDirPath = './plots/plots/'
    runFileList = ScanDir(runDirPath)
    simLengthFiles = []
    scoreFiles = []
    for runFile in runFileList:
        if"predator" in runFile:
            if "run/" in runFile and not FileSyncDirectoryCheck(runFile, runDirPath, runPlotDirPath):
                # runPlot.createPlots(runFile, runDirPath, runPlotDirPath)
                if "s8" in runFile:
                    simLengthFiles.append(runFile)
                # break
                pass
            elif "evolution/" in runFile and not FileSyncDirectoryCheck(runFile, runDirPath, runPlotDirPath):
                # evPlot.createPlots(runFile, runDirPath, runPlotDirPath)
                # break
                if "s8" in runFile:
                    scoreFiles.append(runFile)
                pass
    mapType = {
        "T1": "Przypadek 1",
        "T2": "Przypadek 2",
        "T3": "Przypadek 3",
        "T4": "Przypadek 4"
        }
    predScores = {}
    preyScores = {}
    meanLength = {}
    for k, v in mapType.items():
        file = None
        for scoreFile in scoreFiles:
            if k in scoreFile:
                file=scoreFile
        # print(v, file)
        predData = openFile(file)
        preyData = openFile(file.replace("predator", "prey"))
        # if len(predData.keys()) < 6:
        #     break
        predScores[v] = predData[3].tail(10).mean()
        preyScores[v] = preyData[3].tail(10).mean()

        simLengths = np.array([])
        files = [lenFile for lenFile in simLengthFiles if k in lenFile]
        for file in files:
            predData = openFile(file)
            preyData = openFile(file.replace("predator", "prey"))
            if len(predData) > 10:
                simLengths = np.append(simLengths, [len(predData), len(preyData)])

        meanLength[v] = simLengths.mean()*0.05
        # if len(predData.keys()) < 6:
        #     break
        # print(v, file)

    print(predScores)
    print(preyScores)
    print(meanLength)
    data = {"a": 1, "b": 23, "c":100}


    fig = go.Figure(data=[go.Bar(x=list(predScores.keys()), y=list(predScores.values()))])
    FONT_SIZE = 18
    layout = go.Layout(
        yaxis_title = "Średni wynik przystosowania",
        height=600,
        width=1200,
        yaxis=go.YAxis(
            range=[0, 700],
        ),
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    fig.write_image(runPlotDirPath+"predSc.png")

    fig = go.Figure(data=[go.Bar(x=list(preyScores.keys()), y=list(preyScores.values()))])
    FONT_SIZE = 18
    layout = go.Layout(
        yaxis_title = "Średni wynik przystosowania",
        height=600,
        width=1200,
        yaxis=go.YAxis(
            range=[0, 700],
        ),
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    fig.write_image(runPlotDirPath+"preySc.png")

    fig = go.Figure(data=[go.Bar(x=list(meanLength.keys()), y=list(meanLength.values()))])
    FONT_SIZE = 18
    layout = go.Layout(
        yaxis_title = "Średni czas trwania symulacji [s]",
        height=600,
        width=1200,
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    fig.write_image(runPlotDirPath+"simLen.png")
    fig.show()