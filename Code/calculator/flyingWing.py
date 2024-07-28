# Settings for the flying wing.
wcl = 7  # Wing cube loading of the glider.
ar = 3  # Aspect ratio fo the wing.
finAR = 2  # Aspect ratio of the fin.
taper = 0.45  # Taper ratio of the wing.
weight = 0  # You can leave this value empty. Weight in grams of the projected glider.
sweep = 25  # Sweep angle of the leading edge.
elevonChord = 20  # mm of the chord for the elevons.
electronicsWeight = 10  # Estimated weight of the electronics in grams. (Controller = 10 grams, battery = 10 grams, servos = 6 grams).

fr4Mode = True
fr4 = 1.85  # Density of FR4 in g/cm^3
fr4Thickness = 1.2  # Thickness of the PCB

# Following densities are wrong:
# Density of E flute cardboard (1.5 mm) is 1.61 g/dm^2.
# Desnity of D flute cardboard (2 mm) is
# Desnity of C flute cardboard (4 mm) is 5.15 g/dm^2.
# Desnity of B flute cardboard (3 mm) is 4.95 g/dm^2.
# Desnity of A flute cardboard (5 mm) is

# The density of the 2 mm cardboard I have been using is 4.83 grams per dm^2.
materialDensity = 4.83  # Denisty of the material in g/dm^2.

materialWeight = 0
totalWeight = electronicsWeight + materialWeight

i = 0
while i < 10000:
    if totalWeight < weight:
        weight -= 0.01
    if totalWeight > weight:
        weight += 0.01

    sa = (weight / wcl) ** (1 / 1.5)
    saMM = sa * 10000
    span = (ar * saMM) ** (1 / 2)
    chord = span / ar
    rootChord = (2 * chord) / (1 + taper)
    tipChord = rootChord * taper
    finSA = 0.1 * saMM
    finChord = (finAR * finSA) ** (1 / 2)
    finHeight = finChord / finAR
    elevonSA = saMM / 8
    elevonLength = elevonSA / elevonChord
    if fr4Mode == True:
        totalSA = sa + finSA / 10000 * 2 + elevonSA / 10000 * 2
        materialWeight = totalSA * fr4 * fr4Thickness
    else:
        totalSA = sa * 2 * 1.036 + finSA / 10000 * 2 + elevonSA / 10000 * 2
        materialWeight = totalSA * materialDensity
    totalWeight = electronicsWeight + materialWeight

    i += 1

# print("\nFinished optimzing design for minimum weight.")

print("\nSurface area:", sa, "dm^2")
print("Span:", span, "mm")
print("Average chord:", chord, "mm")
print("Tip chord:", tipChord, "mm")
print("Root chord:", rootChord, "mm")
print("Fin surface area:", finSA, "mm^2")
print("Fin chord:", finChord, "mm")
print("Fin height:", finHeight, "mm")
print("Sweep:", sweep, "degrees")
print("Elevon surface area:", elevonSA, "mm^2")
print("Elevon length:", elevonLength, "mm")
print("Elevon chord:", elevonChord, "mm")

print("\nMaterial surface area:", totalSA, "dm^2")
print("Material weight:", materialWeight, "g")
print("Electronics weight:", electronicsWeight, "g")
print("Total weight:", electronicsWeight + materialWeight, "g")
