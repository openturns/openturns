"""
Use case : FireSatellite test function
======================================
"""
import openturns as ot
import math as m


class FireSatelliteModel:
    """
    Data class for the Fire Satellite.

    Attributes
    ----------

    dim : Dimension of the problem
          dim = 9

    H : Altitude (m), :class:`~openturns.Normal` distribution
         First marginal, ot.Normal(18e6,1e6)

    Pother : Power other than ACS (W), :class:`~openturns.Normal` distribution
         Second marginal, ot.Normal(1000.0,50.0)

    Fs : Average solar flux (W/m^2), :class:`~openturns.Normal` distribution
         Third marginal, ot.Normal(1400.0,20.0)

    theta : Deviation of moment axis (deg), :class:`~openturns.Normal` distribution
         Fourth marginal, ot.Normal(15.0,1.0)

    Lsp : Moment arm for radiation torque (m), :class:`~openturns.Normal` distribution
         Fifth marginal, ot.Normal(2.0,0.4)

    q : Reflectance factor (-), :class:`~openturns.Normal` distribution
         Sixth marginal, ot.Normal(0.5,1.0)

    RD : Residual dipole of spacecraft (A.m^2), :class:`~openturns.Normal` distribution
         Seventh marginal, ot.Normal(5.0,1.0)

    Lalpha : Moment arm for aerodynamic torque (m), :class:`~openturns.Normal` distribution
         Eighth marginal, ot.Normal(2.0,0.4)

    Cd : Drag coefficient (-), :class:`~openturns.Normal` distribution
         Nineth marginal, ot.Normal(1.0,0.3)


    distributionX : :class:`~openturns.ComposedDistribution`
                    The joint distribution of the input parameters.

    model : :class:`~openturns.PythonFunction`
               The Fire Satellite model with H, Pother, Fs, theta, Lsp, q, RD, Lalpha and Cd as variables. This function retrieves three outputs : the total torque, the total power and the area of solar array

    modelTotalTorque : :class:`~openturns.PythonFunction`
               The Fire Satellite model retrieving only the  Total Torque as output, with H, Pother, Fs, theta, Lsp, q, RD, Lalpha and Cd as variables.

    modelTotalPower : :class:`~openturns.PythonFunction`
               The Fire Satellite model retrieving only the  Total Power as output, with H, Pother, Fs, theta, Lsp, q, RD, Lalpha and Cd as variables. This function retrieves three outputs : the total torque, the total power and the area of solar array

    modelSolarArrayArea : :class:`~openturns.PythonFunction`
               The Fire Satellite model retrieving only the  Solar Array Area as output, with H, Pother, Fs, theta, Lsp, q, RD, Lalpha and Cd as variables. This function retrieves three outputs : the total torque, the total power and the area of solar array

    c : Speed of light, constant
        c = 2.9979e8 m/s

    omega_max : Maximum rotational velocity of reaction wheel, constant
        omega_max = 6000 rpm

    n : Number of reaction wheels that could be active, constant
        n = 3

    delta_theta_slew : Slewing time period, constant
        delta_theta_slew = 760 s

    As : Area reflecting radiation, constant
        As =  13.85 m^2

    i : Sun incidence angle, constant
        i = 0 deg

    M :  Magnetic moment of earth, constant
        M =  7.96e15 A.m^2

    rho : Atmospheric density, constant
        rho =  5.1480e-11 kg/m^3

    A : Cross-sectional in flight direction, constant
        A =  13.85 m^2

    Phold : Holding power, constant
        Phold = 20 W

    mu :  Earth gravity constant
        mu =  398600.4418e9 m^3/s^2

    Id : Inherent degradation of array, constant
        Id = 0.77

    t : Thickness of solar panels, constant
        t =  0.005 m

    n_sa : Number of solar arrays, constant
        n_sa = 3

    epsilon_deg : Degradation in power production capability, constant
        epsilon_deg = 0.0375 percent per year

    LT : Lifetime of spacecraft, constant
            LT = 15 years

    r_lw : Length to width ratio of solar array, constant
        r_lw =  3

    D : Distance between panels, constant
        D = 2 m

    I_bodyX : Inertia of body, X axis, constant
        I_bodyX = 6200 kg.m^2

    I_bodyY : Inertia of body, Y axis, constant
        I_bodyY =  6200 kg.m^2

    I_bodyZ : Inertia of body, Z axis, constant
        I_bodyZ =  4700 kg.m^2

    rho_sa : Average mass density to arrays, constant
        rho_sa =  700 kg.m^3

    eta :  Power efficiency, constant
        eta =  0.22

    phi_target : Target diameter, constant
        phi_target = 235000 m

    RE : Earth radius, constant
        RE = 6378140 m

    tolFPI : Tolerance on Fixed Point Iteration used in the multidisciplinary analysis
        tolFPI = 1e-3  (deterministic)

    maxFPIIter : Maximum number of iterations of Fixed Point Iteration used in the multidisciplinary analysis
        maxFPIIter = 50  (deterministic)



    Examples
    --------
    >>> from openturns.usecases import fireSatellitefunction
    >>> # Load the FireSatellite model model
    >>> m = fireSatellitefunction.FireSatelliteModel()
    """

    def __init__(self):

        # dimension
        self.dim = 9

        # Altitude
        self.H = ot.Normal(18e6, 1e6)

        # Power other than ACS
        self.Pother = ot.Normal(1000.0, 50.0)

        # Average solar flux
        self.Fs = ot.Normal(1400.0, 20.0)

        # Deviation of moment axis
        self.theta = ot.Normal(15.0, 1.0)

        # Moment arm for radiation torque
        self.Lsp = ot.Normal(2.0, 0.4)

        # Reflectance factor
        self.q = ot.Normal(0.5, 1.0)

        # Residual dipole of spacecraft
        self.RD = ot.Normal(5.0, 1.0)

        # Moment arm for aerodynamic torque
        self.Lalpha = ot.Normal(2.0, 0.4)

        # Drag coefficient
        self.Cd = ot.Normal(1.0, 0.3)

        # Input distribution
        self.distributionX = ot.ComposedDistribution(
            [
                self.H,
                self.Pother,
                self.Fs,
                self.theta,
                self.Lsp,
                self.q,
                self.RD,
                self.Lalpha,
                self.Cd,
            ]
        )

        self.distributionX.setDescription(
            ["H", "Pother", "Fs", "theta", "Lsp", "q", "RD", "Lalpha", "Cd"]
        )

        # Definitions of models
        # Global model
        self.model = ot.PythonFunction(9, 3, self.multidisciplinaryAnalysis)

        # Model of Total torque
        def multidisciplinaryAnalysisToTalTorque(x):
            return [self.multidisciplinaryAnalysis(x)[0]]

        self.modelTotalTorque = ot.PythonFunction(
            9, 1, multidisciplinaryAnalysisToTalTorque
        )

        # Model of Total power
        def multidisciplinaryAnalysisToTalPower(x):
            return [self.multidisciplinaryAnalysis(x)[1]]

        self.modelTotalPower = ot.PythonFunction(
            9, 1, multidisciplinaryAnalysisToTalPower
        )

        # Model of Solar Array Area
        def multidisciplinaryAnalysisSolarArrayArea(x):
            return [self.multidisciplinaryAnalysis(x)[2]]

        self.modelSolarArrayArea = ot.PythonFunction(
            9, 1, multidisciplinaryAnalysisSolarArrayArea
        )

        # Optional variables (deterministic)
        # Speed of light
        self.c = 2.9979e8

        # Maximum rotational velocity of reaction wheel
        self.omega_max = 6000.0

        # Number of reaction wheels that could be active
        self.n = int(3)

        # Slewing time period
        self.delta_theta_slew = 760.0

        # Area reflecting radiation
        self.As = 13.85

        # Sun incidence angle
        self.i = 0.0

        # Magnetic moment of earth
        self.M = 7.96e15

        # Atmospheric density
        self.rho = 5.1480e-11

        # Cross-sectional in flight direction
        self.A = 13.85

        # Holding power
        self.Phold = 20.0

        # Earth gravity constant
        self.mu = 3.98600e14

        # Inherent degradation of array
        self.Id = 0.77

        # Thickness of solar panels
        self.t = 0.005

        # Number of solar arrays
        self.n_sa = int(3)

        # Degradation in power production capability
        self.epsilon_deg = 0.0375

        # Lifetime of spacecraft
        self.LT = 15.0

        # Length to width ratio of solar array
        self.r_lw = 3.0

        # Distance between panels
        self.D = 2.0

        # Inertia of body, X axis
        self.I_bodyX = 6200.0

        # Inertia of body, Y axis
        self.I_bodyY = 6200.0

        # Inertia of body, Z axis
        self.I_bodyZ = 4700.0

        # Average mass density to arrays
        self.rho_sa = 700.0

        # Power efficiency
        self.eta = 0.22

        # Target diameter
        self.phi_target = 235000.0

        # Earth radius
        self.RE = 6378140

        # Fixed Point Iteration tolerance
        self.tolFPI = 1e-3

        # Fixed Point Iteration maximum iteration
        self.maxFPIIter = int(50)

    def power(self, inputs):
        """
        Function computing the power discipline outputs to retrieve the inertia, the total power and the area of solar array

        :inputs: dictionary of inputs of the Power discipline

        """
        theta = m.pi / 180 * inputs["theta"]
        Fs = inputs["Fs"]
        P_ACS = inputs["P_ACS"]
        P_other = inputs["P_other"]
        delta_t_orbit = inputs["delta_t_orbit"]
        delta_t_eclipse = inputs["delta_t_eclipse"]

        n_sa = self.n_sa
        LT = self.LT
        epsilon_deg = self.epsilon_deg
        D = self.D
        r_lw = self.r_lw
        I_bodyX = self.I_bodyX
        I_bodyY = self.I_bodyY
        I_bodyZ = self.I_bodyZ
        Id = self.Id
        rho_sa = self.rho_sa
        t = self.t
        eta = self.eta

        # Total power
        Ptot = P_ACS + P_other

        # power production capability at beginning of life
        P_BOL = eta * Fs * Id * m.cos(theta)

        # power production capability at end of life
        P_EOL = P_BOL * (1 - epsilon_deg) ** (LT)

        # spacecraft power requirement for eclipse
        Pe = Ptot

        # spacecraft power requirement for daylight
        Pd = Ptot

        # Time per orbit spent in eclipse
        Te = delta_t_eclipse

        # Time per orbit spent in sunlight
        Td = delta_t_orbit - Te

        # Required power output
        Xe = 0.6
        Xd = 0.8
        P_sa = (Pe * Te / Xe + Pd * Td / Xd) / Td

        # total array size
        A_sa = P_sa / P_EOL

        # Determination of moment of inertia
        L = m.sqrt(A_sa * r_lw / n_sa)
        W = m.sqrt(A_sa / (r_lw * n_sa))
        m_sa = 2 * rho_sa * L * W * t
        I_saX = m_sa * (1 / 12 * (L**2 + t**2) + (D + L / 2) ** 2)
        I_saY = m_sa / 12 * (t**2 + W**2)
        I_saZ = m_sa * (1 / 12 * (L**2 + W**2) + (D + L / 2) ** 2)

        # total moment of inertia
        I_tot = ot.Sample([[I_saX + I_bodyX], [I_saY + I_bodyY], [I_saZ + I_bodyZ]])
        # Minimal inertia
        I_min = I_tot.getMin()[0]
        # Maximal inertia
        I_max = I_tot.getMax()[0]

        outputs = {}
        outputs["Imax"] = I_max
        outputs["Imin"] = I_min
        outputs["A_sa"] = A_sa
        outputs["P_tot"] = Ptot

        return outputs

    def orbit(self, inputs):
        """
        Function computing the orbit discipline outputs and retrieve the slewing angle, the velocity, the orbit duration and the eclipse duration

        :inputs: dictionary of inputs of the Orbit discipline

        """
        mu = self.mu
        phi_target = self.phi_target
        RE = self.RE

        H = inputs["H"]

        # satellite velocity
        v = m.sqrt(mu / (RE + H))
        # orbit period
        delta_t_orbit = 2 * m.pi * (RE + H) / v
        # maximum eclipse time
        delta_t_eclipse = delta_t_orbit / m.pi * m.asin(RE / (RE + H))
        # maximum slewing angle
        theta_slew = m.atan(
            m.sin(phi_target / RE) / (1 - m.cos(phi_target / RE) + H / RE)
        )

        outputs = {}
        outputs["theta_slew"] = theta_slew
        outputs["v"] = v
        outputs["delta_t_orbit"] = delta_t_orbit
        outputs["delta_t_eclipse"] = delta_t_eclipse

        return outputs

    def attitudeControl(self, inputs):
        """
        Function computing the attitude and control discipline outputs to retrieve the power of ACS and total torque

        :inputs: dictionary of inputs of the Attitude and Control discipline

        """
        delta_theta_slew = self.delta_theta_slew
        mu = self.mu
        c = self.c
        As = self.As
        i = self.i
        M = self.M
        rho = self.rho
        A = self.A
        omega_max = self.omega_max
        n = self.n
        Phold = self.Phold
        RE = self.RE

        theta_slew = inputs["theta_slew"]
        Imax = inputs["Imax"]
        Imin = inputs["Imin"]
        H = inputs["H"]
        theta = m.pi / 180.0 * inputs["theta"]
        L_sp = inputs["L_sp"]
        Fs = inputs["Fs"]
        q = inputs["q"]
        R_D = inputs["R_D"]
        C_d = inputs["C_d"]
        L_alpha = inputs["L_alpha"]
        v = inputs["v"]

        # slewing torque
        tau_slew = 4 * theta_slew * Imax / delta_theta_slew**2

        # torque due to gravity gradients
        tau_g = 3 * mu / (2 * RE + H) ** 3 * abs(Imax - Imin) * m.sin(2 * theta)

        # torque due to solar radiation
        tau_sp = L_sp * Fs / c * As * (1 + q) * m.cos(i)

        # torque due to magnetic fiel interactions
        tau_m = 2 * M * R_D / (RE + H) ** 3

        # torque due to atmospheric drag
        tau_alpha = 0.5 * rho * L_alpha * C_d * A * v**2

        # total disturbance torque
        tau_dist = m.sqrt(tau_sp**2 + tau_m**2 + tau_g**2 + tau_alpha**2)

        # total torque
        tau_tot = ot.Sample([[tau_dist], [tau_slew]]).getMax()[0]

        # attitude power control
        P_ACS = tau_tot * omega_max + n * Phold

        outputs = {}
        outputs["P_ACS"] = P_ACS
        outputs["tau_tot"] = tau_tot

        return outputs

    def multidisciplinaryAnalysis(self, x):
        """
        Function computing the multidisciplinary analysis to retrieve the total torque, the total power and the area of solar array

        :x: list of inputs

        """
        H = x[0]
        Pother = x[1]
        Fs = x[2]
        theta = x[3]
        Lsp = x[4]
        q = x[5]
        RD = x[6]
        Lalpha = x[7]
        Cd = x[8]

        tolP_ACS = 1e10

        # run of orbit discipline
        inputs_orbit = {}
        inputs_orbit["H"] = H

        outputs_orbit = self.orbit(inputs_orbit)

        itFPI = 0

        while tolP_ACS > self.tolFPI and itFPI < self.maxFPIIter:

            # run of power discipline
            inputs_power = {}
            inputs_power["theta"] = theta
            inputs_power["Fs"] = Fs
            inputs_power["P_other"] = Pother
            inputs_power["delta_t_orbit"] = outputs_orbit["delta_t_orbit"]
            inputs_power["delta_t_eclipse"] = outputs_orbit["delta_t_eclipse"]

            global outputs_attitude
            if itFPI == 0:
                inputs_power["P_ACS"] = 150.0
            else:
                inputs_power["P_ACS"] = outputs_attitude["P_ACS"]

            outputs_power = self.power(inputs_power)

            # run of attitude discipline
            inputs_attitude = {}
            inputs_attitude["theta_slew"] = outputs_orbit["theta_slew"]
            inputs_attitude["v"] = outputs_orbit["v"]
            inputs_attitude["Imax"] = outputs_power["Imax"]
            inputs_attitude["Imin"] = outputs_power["Imin"]
            inputs_attitude["H"] = H
            inputs_attitude["theta"] = theta
            inputs_attitude["L_sp"] = Lsp
            inputs_attitude["Fs"] = Fs
            inputs_attitude["q"] = q
            inputs_attitude["R_D"] = RD
            inputs_attitude["C_d"] = Cd
            inputs_attitude["L_alpha"] = Lalpha

            outputs_attitude = self.attitudeControl(inputs_attitude)
            tolP_ACS = abs(inputs_power["P_ACS"] - outputs_attitude["P_ACS"])

            itFPI += 1

        return [
            outputs_attitude["tau_tot"],
            outputs_power["P_tot"],
            outputs_power["A_sa"],
        ]
