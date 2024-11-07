# Based on: https://github.com/nliaudat/magnetometer_calibration/blob/main/calibrate.py.
# Code corrected by S. James Remington, see issue #1 in above contribution.
# Further modified by Charlie Nicholson for better formatting.

import numpy as np
from scipy import linalg
from matplotlib import pyplot as plt

# Settings.
raw_file = "exampleData.csv"  # Required data input file: x,y,z values in .csv (text, comma separated value) format.
show_plots = False  # Config to show plots of raw and calibrated data.
export_results = False  # Config to export the calibrated data to a file.
cal_acc = False  # Config to calibrate the accelerometer or magnetometer.


class Magnetometer(object):
    MField = 1000  # Arbitrary norm of magnetic field vectors.

    def __init__(self, F=MField):
        # Initialize values.
        self.F = F
        self.b = np.zeros([3, 1])
        self.A_1 = np.eye(3)

    def run(self):
        if cal_acc:
            print("\nCalibrating accelerometer data.")
        else:
            print("\nCalibrating magnetometer data.")

        data = np.loadtxt(raw_file, delimiter=",")

        if cal_acc:
            data = data[:, [0, 1, 2]]
        else:
            data = data[:, [3, 4, 5]]

        print("\nShape of data array:", data.shape)
        print("\nFirst 5 rows raw:\n", data[:5])

        # Ellipsoid fit.
        s = np.array(data).T
        M, n, d = self.__ellipsoid_fit(s)

        # Calibration parameters.
        M_1 = linalg.inv(M)
        self.b = -np.dot(M_1, n)
        self.A_1 = np.real(
            self.F / np.sqrt(np.dot(n.T, np.dot(M_1, n)) - d) * linalg.sqrtm(M)
        )

        # Print M, n, d, and related matrices.
        # print("\nData normalized to ", self.F, ".")
        print("\nSoft iron transformation matrix:\n", self.A_1)
        print("\nHard iron bias:\n", self.b)

        plt.rcParams["figure.autolayout"] = True
        # fig = plt.figure()
        # ax = fig.add_subplot(111, projection='3d')
        # ax.scatter(data[:,0], data[:,1], data[:,2], marker='o', color='r')
        # plt.show()

        result = []
        for row in data:
            # Subtract the hard iron offset.
            xm_off = row[0] - self.b[0]
            ym_off = row[1] - self.b[1]
            zm_off = row[2] - self.b[2]

            # Multiply by the inverse soft iron offset.
            xm_cal = (
                xm_off * self.A_1[0, 0]
                + ym_off * self.A_1[0, 1]
                + zm_off * self.A_1[0, 2]
            )
            ym_cal = (
                xm_off * self.A_1[1, 0]
                + ym_off * self.A_1[1, 1]
                + zm_off * self.A_1[1, 2]
            )
            zm_cal = (
                xm_off * self.A_1[2, 0]
                + ym_off * self.A_1[2, 1]
                + zm_off * self.A_1[2, 2]
            )

            result = np.append(result, np.array([xm_cal, ym_cal, zm_cal]))

        result = result.reshape(-1, 3)
        fig = plt.figure()
        ax = fig.add_subplot(111, projection="3d")
        ax.scatter(result[:, 0], result[:, 1], result[:, 2], marker="o", color="g")

        print("\nFirst 5 rows calibrated:\n", result[:5])

        if export_results:
            np.savetxt("out.txt", result, fmt="%f", delimiter=" ,")

        print(
            "\nThe following code can be copy and pasted into the Arduino code. Please remember you calibrated for ",
            end="",
        )
        if cal_acc:
            print("accelerometer data.")

            self.b = np.round(self.b, 2)
            print(
                "\nfloat A_B[3]{",
                self.b[0],
                ",",
                self.b[1],
                ",",
                self.b[2],
                "};",
                "\n",
            )

            self.A_1 = np.round(self.A_1, 5)
            print(
                "float A_Ainv[3][3]{{",
                self.A_1[0, 0],
                ",",
                self.A_1[0, 1],
                ",",
                self.A_1[0, 2],
                "},",
            )
            print("{", self.A_1[1, 0], ",", self.A_1[1, 1], ",", self.A_1[2, 1], "},")
            print("{", self.A_1[2, 0], ",", self.A_1[2, 1], ",", self.A_1[2, 2], "}};")
        else:
            print("magnetometer data.")

            self.b = np.round(self.b, 2)
            print(
                "\nfloat M_B[3]{",
                self.b[0],
                ",",
                self.b[1],
                ",",
                self.b[2],
                "};",
                "\n",
            )

            self.A_1 = np.round(self.A_1, 5)
            print(
                "float M_Ainv[3][3]{{",
                self.A_1[0, 0],
                ",",
                self.A_1[0, 1],
                ",",
                self.A_1[0, 2],
                "},",
            )
            print("{", self.A_1[1, 0], ",", self.A_1[1, 1], ",", self.A_1[2, 1], "},")
            print("{", self.A_1[2, 0], ",", self.A_1[2, 1], ",", self.A_1[2, 2], "}};")

        if show_plots:
            plt.show()

    def __ellipsoid_fit(self, s):
        """Estimate ellipsoid parameters from a set of points.

        Parameters
        ----------
        s : array_like
          The samples (M,N) where M=3 (x,y,z) and N=number of samples.

        Returns
        -------
        M, n, d : array_like, array_like, float
          The ellipsoid parameters M, n, d.

        References
        ----------
        .. [1] Qingde Li; Griffiths, J.G., "Least squares ellipsoid specific
           fitting," in Geometric Modeling and Processing, 2004.
           Proceedings, vol., no., pp.335-340, 2004
        """

        # D (samples).
        D = np.array(
            [
                s[0] ** 2.0,
                s[1] ** 2.0,
                s[2] ** 2.0,
                2.0 * s[1] * s[2],
                2.0 * s[0] * s[2],
                2.0 * s[0] * s[1],
                2.0 * s[0],
                2.0 * s[1],
                2.0 * s[2],
                np.ones_like(s[0]),
            ]
        )

        # S, S_11, S_12, S_21, S_22 (Eq. 11).
        S = np.dot(D, D.T)
        S_11 = S[:6, :6]
        S_12 = S[:6, 6:]
        S_21 = S[6:, :6]
        S_22 = S[6:, 6:]

        # C (Eq. 8, k=4).
        C = np.array(
            [
                [-1, 1, 1, 0, 0, 0],
                [1, -1, 1, 0, 0, 0],
                [1, 1, -1, 0, 0, 0],
                [0, 0, 0, -4, 0, 0],
                [0, 0, 0, 0, -4, 0],
                [0, 0, 0, 0, 0, -4],
            ]
        )

        # v_1 (Eq. 15, solution).
        E = np.dot(linalg.inv(C), S_11 - np.dot(S_12, np.dot(linalg.inv(S_22), S_21)))

        E_w, E_v = np.linalg.eig(E)

        v_1 = E_v[:, np.argmax(E_w)]
        if v_1[0] < 0:
            v_1 = -v_1

        # v_2 (Eq. 13, solution).
        v_2 = np.dot(np.dot(-np.linalg.inv(S_22), S_21), v_1)

        # Quadratic-form parameters, parameters h and f swapped as per correction by Roger R on Teslabs page.
        M = np.array(
            [
                [v_1[0], v_1[5], v_1[4]],
                [v_1[5], v_1[1], v_1[3]],
                [v_1[4], v_1[3], v_1[2]],
            ]
        )
        n = np.array([[v_2[0]], [v_2[1]], [v_2[2]]])
        d = v_2[3]

        return M, n, d


if __name__ == "__main__":
    Magnetometer().run()
