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
        int mx = 20;
        int my = 20;
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

            for (int y = 0; y < level.GetLength(1); y++)
                for (int x = 0; x < level.GetLength(0); x++)
                {
                    level[x, y] = new Cell((int)(x * width), (int)(y * height));
                    scratch[x, y] = new Cell((int)(x * width), (int)(y * height));
                }

            #endregion

            ToolWindows tWindow = new ToolWindows();
            Preview pWindow = new Preview();

            tWindow.Show();
            pWindow.Show();

        }

        /*--------EVENTS------------------------*/

        #region ToolStrip

        #endregion

        #region MenuStrip

        #endregion

        #region Graphics

        private void DrawCall(object sender, PaintEventArgs e)
        {
            float width = ((float)graphicsPanel1.ClientSize.Width) / ((float)level.GetLength(0));
            float height = ((float)graphicsPanel1.ClientSize.Height) / ((float)level.GetLength(1));

            float thickness = 10;

            for (int y = 0; y < level.GetLength(1); y++)
            {
                Pen penY = (y % thickness == 0) ? new Pen(Color.Black, 2) : new Pen(Color.Black);
                e.Graphics.DrawLine(penY, 0, y * height, graphicsPanel1.ClientSize.Width, y * height);
                for (int x = 0; x < level.GetLength(0); x++)
                {
                    Pen penX = (x % thickness == 0) ? new Pen(Color.Black, 2) : new Pen(Color.Black);
                    e.Graphics.DrawLine(penX, x * width, 0, x * width, graphicsPanel1.ClientSize.Height);

                    if (level[x, y].isActive == false)
                        continue;

                    e.Graphics.FillRectangle(new SolidBrush(level[x, y].mColor), x * width, y * height, width, height);
                }
            }
        }

        private void ClickEvent(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                float width = ((float)graphicsPanel1.ClientSize.Width) / ((float)level.GetLength(0));
                float height = ((float)graphicsPanel1.ClientSize.Height) / ((float)level.GetLength(1));

                float px = e.X / width, py = e.Y / height;

                level[(int)px, (int)py].isActive = !level[(int)px, (int)py].isActive;

                graphicsPanel1.Invalidate();
            }
        }

        #endregion

        #region Other Events

        #endregion

        /*---------------------------------------*/

        #region Other

        #endregion

    }
}
