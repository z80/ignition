using Godot;
using System;

public class Terminal : Node2D
{
    [Export]
    public int termWidth;
    [Export]
    public int termHeight;
    [Export]
    public int characterSpacingX;
    [Export]
    public int characterSpacingY;
    [Export]
    public int characterHeight;
    public override void _Ready()
    {
        base._Ready();
    }

    public override void _Process(float delta)
    {
        base._Process(delta);
    }

    public override void _Draw()
    {
        float sizeX = termWidth * characterSpacingX;
        base._Draw();
    }
}
