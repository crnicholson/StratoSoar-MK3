from pykml import parser

kml_file = "Waypoints.kml"

with open(kml_file, mode="r", encoding="utf-8") as f:
    root = parser.parse(f).getroot()

output = (root.Document.Placemark.Polygon.outerBoundaryIs.LinearRing.coordinates).text

output = output.replace("\n", "")
output = output.replace("\t", "")

locations = output.split(" ")

length = len(locations)
locations.pop(length - 1)

finalCoordinates = []

for location in locations:
    coordinates = location.split(",")
    for coordinate in coordinates:
        if float(coordinate) != 0.0:
            finalCoordinates.append(float(coordinate))

print("\nCopy and paste this into coordinates.h. Make sure to replace [] with {}.\n")
print(finalCoordinates)
