# https://www.wescottdesign.com/articles/pid/pidWithoutAPhd.pdf

from math import *
import cmath
import numpy as np
import matplotlib.pyplot as plt

"""

The motor response is represented by the differential equation

y''(t) = 1/tau(kV - y'(t))
y(t) represents the angle position at time t

We can classify this differential equation as a
2nd order linear non-homogeneous ode
y''(t) + y'(t)/tau = kV/tau

Rewrite it into a standard schema
y''(t) + ay'(t) = c where a and c are constants
a = 1/tau
c = kV/tau

The solution to this ode is
y(t) = k1*exp(-a/t)/a + t*(c/a) + k2
where k1 and k2 are constants that is determined by
the initial condition.

With this equation, we can predict how the motor angle will
change if we had induced it with a step response by plugging in
y(0) and y(1) and solving for the constants k1 and k2

V is the voltage induced
k represents the gain in volts/degrees, it says how fast the motor will turn when voltage changes
tau is in seconds, and represent how fast the motor speed will settle to a constant speed when voltage changes

"""

def solve_motor_ivp(tau, k, V, t0, t1):
    a = 1/tau
    c = k*V/tau
    
    A = np.array([[exp(-t0/a)/a, 1],
                  [exp(-t1/a)/a, 1]])
    b = np.array([t0 - c*t0/a,
                  t1 - c*t1/a])
    return np.linalg.solve(A, b)

def make_motor_function(tau, k, V, t0, t1):
    r = solve_motor_ivp(tau, k, V, t0, t1)
    a = 1/tau
    c = k*V/tau
    return lambda t: r[0]*exp(-t/a)/a + c*t/a + r[1]

def plot_motor_position():    
    t0 = 0
    t1 = 1
    xs = np.linspace(t0, t1, 100)

    plt.clf()
    fig, axes = plt.subplots(3, 1) 
    for V in range(1, 11):
        k = 1
        tau = 0.2
        f = make_motor_function(tau, k, V, t0, t1)
        ys = np.array([f(x) for x in xs])
        axes[0].plot(xs, ys, label='V={} k={} tau={}'.format(V, k, tau))
        axes[0].legend()

    for k in range(1, 11):
        V = 9
        f = make_motor_function(tau, k, V, t0, t1)
        ys = np.array([f(x) for x in xs])
        axes[1].plot(xs, ys, label='V={} k={} tau={}'.format(V, k, tau))
        axes[1].legend()

    for tau in range(0, 100, 10):
        if tau == 0:
            tau = 1
        V = 9
        k = 1
        f = make_motor_function(tau, k, V, t0, t1)
        axes[2].plot(xs, ys, label='V={} k={} tau={}'.format(V, k, tau))
        axes[2].legend()

    fig.set_size_inches(18.5, 10.5)
    fig.savefig('motor_position.png', dpi=100)

"""

The non-contact precision actuator has the response of

y''(t) = k_i/m * i
V(t)   = k_t * y(t)

where V(t) is the output transducer

The standard schema for this differential equation is
y''(t) = c where c is a constant
c = k_i/m * i

The general solution is then
y(t) = c*t^2/2 + k1*t + k2

where k1 and k2 are constants determined by initial conditions

k_i is the coil force constant in N/A
k_t is the gain in V/m
m is the total mass of the stage, magnet and the moving portion
i is the current in A

"""
def solve_actuator_ivp(k_i, m, i, t0, t1):
    c = k_i/m * i

    A = np.array([[t0, 1],
                  [t1, 1]])
    b = np.array([t0 - c*t0*t0/2,
                  t1 - c*t1*t1/2])
    return np.linalg.solve(A, b)

def make_actuator_function(k_t, k_i, m, i, t0, t1):
    c = k_i/m * i
    r = solve_actuator_ivp(k_i, m, i, t0, t1)
    return lambda t: k_t*(c*t*t/2 + r[0]*t + r[1])

def plot_actuator_position():
    t0 = 0
    t1 = 1
    k_g = 1
    k_i = 1
    m = 1
   
    plt.clf()
    fig, axes = plt.subplots(2, 1)
    xs = np.linspace(t0, t1, 100)

    for i in range(0, 10):
        f = make_actuator_function(k_g, k_i, m, i, t0, t1)
        ys = np.array([f(x) for x in xs])
        axes[0].plot(xs, ys, label='k_g={} k_i={} m={} i={}'.format(k_g, k_i, m, i))
        axes[0].legend()

    for m in range(1, 10):
        i = 0.5
        f = make_actuator_function(k_g, k_i, m, i, t0, t1)
        ys = np.array([f(x) for x in xs])
        axes[1].plot(xs, ys, label='k_g={} k_i={} m={} i={}'.format(k_g, k_i, m, i))
        axes[1].legend()
    fig.set_size_inches(18.5, 10.5)
    fig.savefig('actuator_position.png', dpi=100)

plot_motor_position()
plot_actuator_position()