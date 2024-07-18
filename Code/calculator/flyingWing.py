# Settings for the flying wing.
wcl = 7  # Wing cube loading of the glider.
ar = 6  # Aspect ratio fo the wing.
finAR = 2  # Aspect ratio of the fin.
taper = 0.45  # Taper ratio of the wing.
weight = 200  # Weight in grams of the projected glider.
sweep = 25  # Sweep angle of the leading edge.
elevonChord = 20 # mm of the chord for the elevons.

# Density of E flute cardboard is 1.61 g/dm^2. 
# Desnity of D flute cardboard is 
materialDensity = 1.61 # Denisty of the material in g/dm^2. 

sa = (weight / wcl) ** (1 / 1.5)

saMM = sa * 10000

span = (ar * saMM) ** (1 / 2)

chord = span / ar

rootChord = (2 * chord) / (1 + taper)

tipChord = rootChord * taper

finSA = 0.1 * saMM

finChord = (finAR * finSA) ** (1 / 2)

finHeight = finChord / finAR

totalSA = sa * 2 * 1.036

materialWeight = totalSA * materialDensity

elevonSA = saMM / 8

elevonLength = elevonSA / elevonChord 

print("Surface area:", sa, "dm^2")
print("Span:", span, "mm")
print("Chord:", chord, "mm")
print("Tip chord:", tipChord, "mm")
print("Root chord:", rootChord, "mm")
print("Fin surface area:", finSA, "mm^2")
print("Fin chord:", finChord, "mm")
print("Fin height:", finHeight, "mm")
print("Sweep:", sweep, "degrees")
print("Material surface area:", totalSA, "dm^2")
print("Material weight:", materialWeight, "g")
print("Elevon surface area:", elevonSA, "mm^2")
print("Elevon length:", elevonLength, "mm")
print("Elevon chord:", elevonChord, "mm")