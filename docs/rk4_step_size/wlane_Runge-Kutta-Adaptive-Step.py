from vpython import *
#GlowScript 2.7 VPython

graph(fast=False)
fun_graph = gcurve(color=color.red, markers=True)

def fun(t,x):
    # This is the right-hand side of the first-order ordinary differential 
    # equation dx/dt = fun.
#    fun = 3*t**2 
    fun = exp(x)-x*exp(t) 
    return fun
    
# Set initial conditions.
t = 0
x = 0.02

# Set initial step size.
dt = 1e-1

# Set minimal step size.
dt_min = 1e-3

# Set relative change tolerances.
dx_max = 0.01  # Enables faster speed.
dx_min = 0.008 # Controls accuracy.
x_tol = 1e-3

while (t < 5):
    rate(100)
    
    # Calculate partial steps.
    k1 = fun(t,      x)
    k2 = fun(t+dt/2, x+dt*k1/2)
    k3 = fun(t+dt/2, x+dt*k2/2)
    k4 = fun(t+dt,   x+dt*k3)
    # Combine partial steps.
    step_x = x + dt/6*(k1+2*k2+2*k3+k4)

    # Calculate partial steps.
    k2 = fun(t+dt/4, x+dt*k1/4)
    k3 = fun(t+dt/4, x+dt*k2/4)
    k4 = fun(t+dt/2, x+dt*k3/2)
    # Combine partial steps.
    half_step_x = x + dt/12*(k1+2*k2+2*k3+k4)

    # Calculate partial steps.
    k2 = fun(t+dt,   x+dt*k1)
    k3 = fun(t+dt,   x+dt*k2)
    k4 = fun(t+2*dt, x+2*dt*k3)
    # Combine partial steps.
    dble_step_x = x + dt/3*(k1+2*k2+2*k3+k4)

    if (abs(step_x) < x_tol): # Use a fixed step size for small values of x.
        if (dt != dt_min):
            print("New step size",dt_min)
            dt = dt_min
        new_x = step_x
    else:
        if (abs(step_x) > x_tol and abs(step_x-half_step_x)/abs(step_x) > dx_max):
            dt = dt/2 # Error is too large; decrease step size.
            print("New step size",dt)
            new_x = half_step_x
        else if (abs(step_x) > x_tol and abs(step_x-dble_step_x)/abs(step_x) < dx_min):
            dt = dt*2 # Larger error is acceptable; increase step size.
            print("New step size",dt)
            new_x = dble_step_x
        else:
            new_x = step_x # This step size is just right.

    x = new_x
    t = t + dt
    
    
    fun_graph.plot(pos=(t,x))