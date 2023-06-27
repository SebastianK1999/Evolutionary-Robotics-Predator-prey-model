import os

import plotly.graph_objects as go
import pandas as pd
import numpy as np

SKIP_IF_EXISTS = True

FONT_SIZE = 24

def openFile(file):
    data = pd.DataFrame(np.loadtxt(file))
    return data

def isAllZero(data):
    for d in data:
        if d != 0:
            return False
    return True

# def createStepsPlot(dir, fileSuffix, predData, preyData):
#     fig = go.Figure()
#     if not isAllZero(predData[0]):
#         fig.add_trace ( go.Scatter (
#             x=[i+1 for i in range(len(predData[0]))],
#             y=predData[0],
#             mode="markers",
#             marker=dict(
#                 symbol="x",
#                 size=15,
#                 angleref="previous",
#             ),
#             name="Liczba kroków symluacj (Łowca)",
#         ))

#     if not isAllZero(preyData[0]):
#         fig.add_trace ( go.Scatter (
#             x=[i+1 for i in range(len(preyData[0]))],
#             y=preyData[0],
#             mode="markers",
#             marker=dict(
#                 symbol="x",
#                 size=15,
#                 angleref="previous",
#             ),
#             name="Liczba kroków symluacj (Ofiara)",
#         ))

#     layout = go.Layout(
#         xaxis_title = "Numer pokolenia etapu",
#         yaxis_title = "Liczba kroków symluacj",
#         height=1000,
#         width=1500,
#     )
#     fig.update_layout(layout)
#     if not os.path.exists(str(dir)+"evSteps"+fileSuffix) or not SKIP_IF_EXISTS:
#         fig.write_image(str(dir)+"evSteps"+fileSuffix)
#     # print( fileSuffix )
#     # fig.show()

# def createRotationPlot(dir, fileSuffix, predData, preyData):
#     fig = go.Figure()
#     if not isAllZero(predData[1]):
#         fig.add_trace ( go.Scatter (
#             x=[i+1 for i in range(len(predData[0]))],
#             y=predData[1],
#             mode="markers",
#             marker=dict(
#                 symbol="x",
#                 size=15,
#                 angleref="previous",
#             ),
#             name="Wynik dopasowania obrotu (Łowca)",
#         ))

#     if not isAllZero(preyData[1]):
#         fig.add_trace ( go.Scatter (
#             x=[i+1 for i in range(len(preyData[0]))],
#             y=preyData[1],
#             mode="markers",
#             marker=dict(
#                 symbol="x",
#                 size=15,
#                 angleref="previous",
#             ),
#             name="Wynik dopasowania obrotu (Ofiara)",
#         ))

#     layout = go.Layout(
#         xaxis_title = "Numer pokolenia etapu",
#         yaxis_title = "Wynik dopasowania obrotu",
#         height=1000,
#         width=1500,
#     )
#     fig.update_layout(layout)
#     if not os.path.exists(str(dir)+"evRot"+fileSuffix) or not SKIP_IF_EXISTS:
#         fig.write_image(str(dir)+"evRot"+fileSuffix)
#     # print( fileSuffix )
#     # fig.show()

def createEvPlot(dir, fileSuffix, predData, preyData, dIndex, name, short):
    if dIndex == 2:
        predData[2] = pd.Series([d/2 for d in predData[2]])
        preyData[2] = pd.Series([d/2 for d in preyData[2]])
    fig = go.Figure()
    if not isAllZero(predData[dIndex]):
        fig.add_trace ( go.Scatter (
            x=[i+1 for i in range(len(predData[dIndex]))],
            y=predData[dIndex],
            mode="markers",
            marker=dict(
                symbol="x",
                size=15,
                angleref="previous",
                color='#EF553B',
            ),
            name="Łowca",
        ))

    if not isAllZero(preyData[dIndex]):
        fig.add_trace ( go.Scatter (
            x=[i+1 for i in range(len(preyData[dIndex]))],
            y=preyData[dIndex],
            mode="markers",
            marker=dict(
                symbol="x",
                size=15,
                angleref="previous",
                color='#00CC96',
            ),
            name="Ofiara",
        ))

    layout = go.Layout(
        xaxis_title = "Numer pokolenia",
        yaxis_title = name,
        height=800,
        width=1200,
        font=dict(
            family="Helvet, monospace",
            size=FONT_SIZE
        )
    )
    fig.update_layout(layout)
    if not os.path.exists(str(dir)+short+fileSuffix) or not SKIP_IF_EXISTS:
        fig.write_image(str(dir)+short+fileSuffix)
    # print( fileSuffix )
    # fig.show()

def createPlots(file, sourceRoot: str, targetRoot: str):
    if os.stat(file).st_size == 0:
        print("Empty",file)
        return
    predData = openFile(file)
    preyData = openFile(file.replace("predator", "prey"))

    sourceExtension = ".txt"
    targetExtension = ".png"
    file = file[len(sourceRoot):-len(sourceExtension)]
    targetFile = targetRoot + file + targetExtension
    dir = os.path.splitext(str(targetFile).replace("predator_",""))[0] 
    fileSuffix = "_" + os.path.split(str(dir))[1] +"_"+ os.path.split(os.path.split(str(dir))[0])[1] + ".png"
    dir = dir + "/"
    # createStepsPlot(dir, fileSuffix, predData, preyData)
    # createRotationPlot(dir, fileSuffix, predData, preyData)
    # createDistancePlot(dir, fileSuffix, predData, preyData)
    createEvPlot(dir, fileSuffix, predData, preyData, 0, "Liczba kroków symluacj ", "evSteps")
    createEvPlot(dir, fileSuffix, predData, preyData, 1, "Wynik przystosowania obrotu", "evRot")
    createEvPlot(dir, fileSuffix, predData, preyData, 2, "Wynik przystosowania odległości", "evDist")
    createEvPlot(dir, fileSuffix, predData, preyData, 3, "Wynik przystosowania", "evSc")

