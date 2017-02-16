using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LevelEditor
{
    public partial class Object : Form
    {
        int mx = 10, my = 10;
        Cell[,] objGrid;
        GameObject mObj;

        public Object()
        {
            InitializeComponent();

            objGrid = new Cell[mx, my];
        }

        private void OnApply(object sender, EventArgs e)
        {

        }

        private void DrawCall(object sender, PaintEventArgs e)
        {

        }
    }
}
