using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace LevelEditor
{
    public partial class GraphicsPanel : Panel
    {
        public GraphicsPanel()
        {
            this.DoubleBuffered = true;
            SetStyle(ControlStyles.ResizeRedraw, true);
        }
    }

    public class Cell
    {

        public Point mPoint;
        public short mID;
        public bool isActive = false;
        public Color mColor;

        public Cell(int _x, int _y)
        {
            mPoint.X = _x;
            mPoint.Y = _y;
            mID = 0;
            mColor = Color.Black;
        }
        public Cell(Point _p)
        {
            mPoint = _p;
            mID = 0;
            mColor = Color.Black;
        }
        public Cell(Point _p, short _id, Color _col)
        {
            mPoint = _p;
            mID = _id;
            mColor = _col;
        }
    }

    public class GameObject
    {
        Cell[,] objGrid;
        //Base Object stuff
        String name;
        GameObject parent;
        ushort uID, ID;


    }

    public class BoolArgs : EventArgs
    {
        public bool boolChange = true;

        public BoolArgs(bool change)
        {
            boolChange = change;
        }
    }


}
