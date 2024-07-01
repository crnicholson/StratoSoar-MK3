import drawsvg as draw

# Settings for the flying wing.
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio fo the wing.
finAR = 2  # Aspect ratio of the fin.
taper = 0.45  # Taper ratio of the wing.
weight = 200  # Weight in grams of the projected glider.
sweep = 25  # Sweep angle of the leading edge.

sa = (weight / wcl) ** (1 / 1.5)

saMM = sa * 10000

span = (ar * saMM) ** (1 / 2)

chord = span / ar

rootChord = (2 * chord) / (1 + taper)

tipChord = rootChord * taper

finSA = 0.1 * saMM

finChord = (finAR * finSA) ** (1 / 2)

finHeight = finChord / finAR

print("Surface area:", sa, "dm^2")
print("Span:", span, "mm")
print("Chord:", chord, "mm")
print("Tip chord:", tipChord, "mm")
print("Root chord:", rootChord, "mm")
print("Fin surface area:", finSA, "mm^2")
print("Fin chord:", finChord, "mm")
print("Fin height:", finHeight, "mm")
print("Sweep:", sweep, "degrees")

d = draw.Drawing(200, 100, origin="center")

d.append(
    draw.Lines(
        -80, 45, 70, 49, 95, -49, -90, -40, close=False, fill="#eeee00", stroke="black"
    )
)

d