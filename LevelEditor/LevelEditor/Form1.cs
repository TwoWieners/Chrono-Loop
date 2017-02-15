using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LevelEditor
{
    public partial class Form1 : Form
    {
        #region Fields
        //Array Size
        int mx = 200;
        int my = 200;
        //Object Counter
        int objCount = 0;

        //Arrays
        Cell[,] level;
        Cell[,] scratch;

        #endregion

        public Form1()
        {
            InitializeComponent();

            #region Initializing

            level = new Cell[mx, my];
            scratch = new Cell[mx, my];

            float width = ((float)graphicsPanel1.ClientSize.Width) / ((float)level.GetLength(0));
            float height = ((float)graphicsPanel1.ClientSize.Height) / ((float)level.GetLength(1));

            for(int y = 0; y < level.GetLength(1); y++)
                for(int x = 0; x < level.GetLength(0); x++)
                {
                    level[x, y] = new Cell((int)(x * width), (int)(y * height));
                    scratch[x, y] = new Cell((int)(x * width), (int)(y * height));
                }

            #endregion
        }

        /*--------EVENTS------------------------*/

        #region ToolStrip

        #endregion

        #region MenuStrip

        #endregion

        #region Graphics

        private void DrawCall(object sender, PaintEventArgs e)
        {

        }

        #endregion

        #region Other Events

        #endregion

        /*---------------------------------------*/

        #region Other

        #endregion

    }
}
