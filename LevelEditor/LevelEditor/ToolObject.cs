﻿using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace LevelEditor
{
    class ToolObject
    {
        private CustomVertex.PositionNormalTextured[] mVertices;
        private int[] mIndices;
        private IndexBuffer mIndexBuffer;
        private VertexBuffer mVertexBuffer;
        private Matrix mTransform;
        private Device mDevice;
        #region Properties
        public CustomVertex.PositionNormalTextured[] Vertices
        {
            get { return mVertices; }
            set { mVertices = value; }
        }

        public int[] Indices
        {
            get { return mIndices; }
            set { mIndices = value; }
        }

        public IndexBuffer IndexBuffer
        {
            get { return mIndexBuffer; }
            set { mIndexBuffer = value; }
        }

        public VertexBuffer VertexBuffer
        {
            get { return mVertexBuffer; }
            set { mVertexBuffer = value; }
        }

        public Matrix Transform
        {
            get { return mTransform; }
            set { mTransform = value; }
        }
        #endregion

        public ToolObject(ref Device _Device)
        {
            mTransform = Matrix.Identity;
            mDevice = _Device;
            MakeGrid();
            VertexDeclaration();
            IndicesDeclaration();
        }
        public ToolObject(string _File, ref Device _Device)
        {
            mTransform = Matrix.Identity;
            mDevice = _Device;
            Load(_File);
            VertexDeclaration();
            IndicesDeclaration();
        }
        public void Load(string _File)
        {
            List<Vector3> Verts = new List<Vector3>();
            List<Vector3> Norms = new List<Vector3>();
            List<Vector2> UVs = new List<Vector2>();
            List<CustomVertex.PositionNormalTextured> vertices = new List<CustomVertex.PositionNormalTextured>();
            List<int> Ind = new List<int>();
            StreamReader sr = new StreamReader(_File);
            string line = string.Empty;
            while ((line = sr.ReadLine()) != null)
            {
                string[] parts = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

                if (parts.Length > 0)
                {
                    Vector3 vec = new Vector3();
                    switch (parts[0])
                    {
                        case "v":
                            vec.X = (float)Convert.ToDouble(parts[1]);
                            vec.Y = (float)Convert.ToDouble(parts[2]);
                            vec.Z = (float)Convert.ToDouble(parts[3]);
                            Verts.Add(vec);
                            break;
                        case "vn":
                            vec.X = (float)Convert.ToDouble(parts[1]);
                            vec.Y = (float)Convert.ToDouble(parts[2]);
                            vec.Z = (float)Convert.ToDouble(parts[3]);
                            Norms.Add(vec);
                            break;
                        case "vt":
                            Vector2 uv = new Vector2();
                            uv.X = (float)Convert.ToDouble(parts[1]);
                            uv.Y = (float)Convert.ToDouble(parts[2]);
                            UVs.Add(uv);
                            break;
                        case "f":
                            for (int i = 1; i < 4; i++)
                            {
                                string[] points = parts[i].Split(new char[] { '/' }, StringSplitOptions.None);
                                Ind.Add(vertices.Count);
                                vertices.Add(new CustomVertex.PositionNormalTextured(Verts[Convert.ToInt32(points[0]) - 1],
                                    Norms[Convert.ToInt32(points[2]) - 1],
                                    UVs[Convert.ToInt32(points[1]) - 1].X,
                                    UVs[Convert.ToInt32(points[1]) - 1].Y));
                            }
                            break;
                        default:
                            break;

                    }
                }
            }
            mVertices = vertices.ToArray();
            mIndices = Ind.ToArray();
        }
        public void MakeGrid()
        {
            List<CustomVertex.PositionNormalTextured> verts = new List<CustomVertex.PositionNormalTextured>();
            List<int> ind = new List<int>();
            for (int i = 0; i <= 20; i++)
            {
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3((float)i - 10, 0, -10), new Vector3(), 0, 0));
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3((float)i - 10, 0, 10.0f), new Vector3(), 0, 0));
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3((float)i  - 10, 0, -10), new Vector3(), 0, 0));
            }
            for (int i = 0; i <= 20; i++)
            {
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3(-10, 0, (float)i - 10), new Vector3(), 0, 0));
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3(10.0f, 0, (float)i - 10), new Vector3(), 0, 0));
                ind.Add(verts.Count);
                verts.Add(new CustomVertex.PositionNormalTextured(new Vector3(-10, 0, (float)i - 10), new Vector3(), 0, 0));
            }
            mVertices = verts.ToArray();
            mIndices = ind.ToArray();
        }
        public void VertexDeclaration()
        {
            mVertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionNormalTextured), mVertices.Length, mDevice, Usage.Dynamic | Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default);
            mVertexBuffer.SetData(mVertices, 0, LockFlags.None);
        }
        public void IndicesDeclaration()
        {
            mIndexBuffer = new IndexBuffer(typeof(int), mIndices.Length * sizeof(int), mDevice, Usage.WriteOnly, Pool.Default);
            mIndexBuffer.SetData(mIndices, 0, LockFlags.None);
        }
        public void Translate(Vector3 _Offset)
        {
            mTransform = Matrix.Translation(_Offset) * mTransform;
        }
        public void Rotate(Vector3 _Rotation)
        {
            mTransform = Matrix.RotationX(_Rotation.X) * mTransform;
            mTransform = Matrix.RotationY(_Rotation.Y) * mTransform;
            mTransform = Matrix.RotationZ(_Rotation.Z) * mTransform;
        }
        public void Scale(Vector3 _Scale)
        {
            mTransform = Matrix.Scaling(_Scale) * mTransform;
        }
    }
}
