# Utility classes and methods for use with the plot digitizer

import numpy as np
import scipy.linalg as linalg

# Copy of method in plotdigitizer.py
def find_lines(ax,label):
    lines = ax.get_lines() # All lines plotted; includes axis/frame lines
    for l in lines:
        if l.get_label() == label:
            return l
    return None

def solve_transformation(p1,p2,p3,p1T,p2T,p3T):
    # p1,...,p3T are original and transformed (T) coordinates in homogeneous vector form
    P = np.hstack([p1,p2,p3]) # Stack horizontally
    PT = np.hstack([p1T,p2T,p3T])

    # Because last coordinate is always 1, system is singular. Need to use pseudo inverse.
    trans_mat = np.matmul(PT,linalg.inv(P))
    return trans_mat



# Transformation class
class Transformation:
    def __init__(self,matrix=np.array([[1,0,0],[0,1,0],[0,0,1]]) ):
        self.matrix = matrix

    def transform(self,p1):
        if isinstance(p1,Coordinate):
            point = p1.transform(self.matrix)
            return point
        else:
            print("Error: Point is not of type Coordinate")
    def inverse_transform(self,p1):
        if isinstance(p1,Coordinate):
            return p1.transform(np.linalg.inv(self.matrix))

    def __str__(self):
        return str(self.matrix)

    def translate(self,dx,dy):
        # Right-multiply translation matrix
        self.matrix = np.matmul(self.matrix, self._get_translate_matrix(dx,dy))
    def rotate(self,angle):
        self.matrix = np.matmul(self.matrix, self._get_rotate_matrix(angle))
    def scale(self,scale_factor_x,scale_factor_y):
        self.matrix = np.matmul(self.matrix, self._get_scale_matrix(scale_factor_x,scale_factor_y))
    def horiz_shear(self,slope):
        self.matrix = np.matmul(self.matrix, self._get_horiz_shear_matrix(slope))

    def _get_translate_matrix(self,dx,dy):
        return np.array([[1,0,dx],[0,1,dy],[0,0,1]])
    def _get_rotate_matrix(self,angle): # assuming angle in degrees
        theta = angle*np.pi/180
        return np.array([[np.cos(theta), np.sin(theta),0],[-np.sin(theta), np.cos(theta),0],[0,0,1]])
    def _get_scale_matrix(self,scale_factor_x,scale_factor_y):
        return np.array([[scale_factor_x,0,0],[0,scale_factor_y,0],[0,0,1]])
    def _get_horiz_shear_matrix(self,slope):
        return np.array([[1,slope,0],[0,1,0],[0,0,1]])

# Coordinate class
# Supports vector addition and scalar multiplication/division
class Coordinate:
    def __init__(self,x,y):
        if isinstance(x,int) or isinstance(x,float):
            if isinstance(y,int) or isinstance(y,float):
                self.x = x
                self.y = y
            else:
                print("error: received unexpected types for x and y")
                self.x=0
                self.y=0
        else:
            if isinstance(x,str) and isinstance(y,str):
                self.x = float(x)
                self.y = float(y)
            else:
                print("error: received unexpected types for x and y")
                self.x=0
                self.y=0
    def __str__(self):
        return "("+str(self.x)+", "+str(self.y)+")"
    def __add__(self, other):
        if isinstance(other,Coordinate):
            return Coordinate(self.x+other.x, self.y+other.y)
        elif isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x+other, self.y+other)
        else:
            return NotImplemented
    def __sub__(self, other):
        if isinstance(other,Coordinate):
            return Coordinate(self.x-other.x, self.y-other.y)
        elif isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x-other, self.y-other)
        else:
            return NotImplemented
    def __mul__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x*other, self.y*other)
        else:
            return NotImplemented
    def __truediv__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x/other, self.y/other)
        else:
            return NotImplemented
    def __floordiv__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x//other, self.y//other)
        else:
            return NotImplemented
    def __radd__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x+other, self.y+other)
        else:
            return NotImplemented
    def __rsub__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(other-self.x, other-self.y)
        else:
            return NotImplemented
    def __rmul__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(self.x*other, self.y*other)
        else:
            return NotImplemented
    def __rtruediv__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(other/self.x, other/self.y)
        else:
            return NotImplemented
    def __rfloordiv__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Coordinate(other/self.x, other/self.y)
        else:
            return NotImplemented
    def __neg__(self):
        return Coordinate(-self.x,-self.y)
    def __abs__(self):
        return self.get_distance(Coordinate(0,0))

    # Calculate Euclidean distance
    def get_distance(self,p2):
        return np.sqrt( np.power(p2.x-self.x,2) + np.power(p2.y-self.y,2) )

    # Convert to homogeneous coordinates vector
    def get_point_homog_vector(self):
        return np.array([[self.x],[self.y],[1]])

    # Convert to column vector (note: avoid this for transformations when translation is needed)
    def get_point_vector(self):
        return np.array([[self.x],[self.y]])

    # Return coordinate transformed by trans_mat
    def transform(self,trans_mat):
        if trans_mat.shape == (3,3):
            ptrans = np.matmul(trans_mat,self.get_point_homog_vector())
            return Coordinate(ptrans[0][0],ptrans[1][0])
        elif trans_mat.shape == (2,2):
            print("Warning: 2x2 transformation matrix will be used in place of homogeneous coordinates.")
            ptrans = np.matmul(trans_mat,self.get_point_vector())
            return Coordinate(ptrans[0][0],ptrans[1][0])

class DatasetParams:
    # Settings for a dataset
    def __init__(self,label,marker='.',markercolor=[1.0,0.0,0.0],markersize=8):
        self.label = label
        self.marker = marker
        self.markercolor = markercolor
        self.markersize = markersize

# Dataset class
# A simple container for Coordinates
# Also allows simple plotting, transformation, appending, undo/redo, and deleting
class Dataset:
    def __init__(self,params,points=[]):
        self.label = params.label
        self.marker = params.marker
        self.markercolor = params.markercolor
        self.markersize = params.markersize

        self.points = points
        self.point_stack = [] # undo stack for un-adding/re-adding points
    
    def __str__(self):
        s = ''
        for p in self.points:
            s = s+str(p)+'\n'
        return s

    def __len__(self):
        return len(self.points)

    def get_params(self):
        return DatasetParams(self.label,self.marker,self.markercolor,self.markersize)

    def load_params(self,ax,params):
        # Simple settings
        old_params = self.get_params()
        self.marker = params.marker
        self.markercolor = params.markercolor
        self.markersize = params.markersize

        # Label (requires changing plot label if already plotted)
        self.prev_label = self.label
        if params.label != self.label:
            l = find_lines(ax,self.label) # Check for existing line
            other = find_lines(ax,params.label) # Check if name is taken
            if other != None:
                print("Error: Label already in use.")
                self.load_params(old_params)
                return False

            if l == None:
                self.label = params.label # Not plotted yet, just change the label
            else:
                self.label = params.label

        return True
    
    def update_axes_label(self,ax):
        l = find_lines(ax,self.prev_label)
        if l == None:
            return
        else:
            l.set_label(self.label)


    def sort_dataset(self,axis='x',direction='increasing'):
        sorted_points = []
        if axis == 'x':
            # Sort by x coordinates
            if direction == 'increasing':
                sorted_points = sorted(self.points, key=lambda k: [k.y, k.x], reverse=False)
            elif direction == 'decreasing':
                sorted_points = sorted(self.points, key=lambda k: [k.y, k.x], reverse=True)
            self.points = sorted_points
        if axis == 'x':
            # Sort by y coordinates
            if direction == 'increasing':
                sorted_points = sorted(self.points, key=lambda k: [k.x, k.y], reverse=False)
            elif direction == 'decreasing':
                sorted_points = sorted(self.points, key=lambda k: [k.x, k.y], reverse=True)
            self.points = sorted_points


    def append_point(self,point):
        self.points.append(point)

    def delete_point(self,loc):
        # Delete point closest to loc, if any
        near_points = []
        for p in self.points:
            if p.get_distance(loc) < 10:
                near_points.append(p)
        min_dist = near_points[0].get_distance(loc)
        closest_point = near_points[0]
        for p in near_points:
            d = p.get_distance(loc)
            if d < min_dist:
                min_dist = d
                closest_point = p
        self.points.remove(closest_point)

    def nudge_last_point(self,direction):
        p = self.points[-1]
        if direction=="up":
            self.points[-1] = Coordinate(p.x,p.y-1)
        elif direction=="down":
            self.points[-1] = Coordinate(p.x,p.y+1)
        elif direction=="left":
            self.points[-1] = Coordinate(p.x-1,p.y)
        elif direction=="right":
            self.points[-1] = Coordinate(p.x+1,p.y)
        else:
            print("Error: Unknown nudge direction")

    def undo_append_point(self):
        self.point_stack.append(self.points[-1])
        self.points = self.points[:-1]
    def redo_append_point(self):
        if len(self.point_stack) > 0:
            self.points.apend(self.point_stack[-1])
            self.point_stack = self.point_stack[:-1]
    def get_xdata(self):
        xdata = []
        for p in self.points:
            xdata.append(p.x)
        return xdata
    def get_ydata(self):
        ydata = []
        for p in self.points:
            ydata.append(p.y)
        return ydata

    # Plot this dataset on the given axes
    # Uses the dataset label to find if we've plotted this data yet
    # If so, data is updated. If not, a new data series is added with the right label.
    def plot_dataset(self,ax,canvas,markersize=None):
        # Check if this dataset (with this label) has been added to the plot yet
        l = find_lines(ax,self.label)
        if l == None:
            # Add this dataset to the plot
            lines = ax.plot(self.get_xdata(),self.get_ydata())
            l = lines[0]
            l.set_linestyle('')
            l.set_marker(self.marker)
            if markersize==None:
                l.set_markersize(self.markersize)
            else:
                l.set_markersize(markersize) # Override
            l.set_markeredgewidth(1)
            l.set_markerfacecolor(self.markercolor)
            l.set_markeredgecolor(self.markercolor)
            l.set_label(self.label) # So we can find this again
        else:
            # Set the data to the current data
            l.set_data(self.get_xdata(),self.get_ydata())
            l.set_marker(self.marker)
            l.set_markerfacecolor(self.markercolor)
            l.set_markeredgecolor(self.markercolor)
            if markersize == None:
                l.set_markersize(self.markersize)
            else:
                l.set_markersize(markersize) # Override
        canvas.draw()





