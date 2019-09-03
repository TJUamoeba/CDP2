package com.example.esp_app;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import lecho.lib.hellocharts.gesture.ContainerScrollType;
import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.model.ValueShape;
import lecho.lib.hellocharts.model.Viewport;
import lecho.lib.hellocharts.view.LineChartView;


public class HumitureActivity extends SpinnerActicity {
    public List<Line> lines=new ArrayList<>();
    public List<PointValue> TemPointValueList =new ArrayList<>();//温度点数据数组
    public List<PointValue> WatPointValueList =new ArrayList<>();//湿度点数据数组
    public List<PointValue> points=new ArrayList<>();//点数组
    public int position=0;
    public Timer timer;
    public Axis axisX=new Axis();//横坐标
    public Axis axisY=new Axis();//纵坐标
    public LineChartView mChartView;
    public LineChartView lineChartView;
    public LineChartData data =new LineChartData();
    public Random random=new Random();
    private ImageButton but;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_humiture);
        but=(ImageButton)findViewById(R.id.image_humiture);
        mChartView=(LineChartView)findViewById(R.id.humiture_chart);

        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(HumitureActivity.this,SpinnerActicity.class);
                startActivity(intent);
            }
        });
        PrintHumitureLine();
        timer=new Timer();
    }

    @Override
    protected void onResume(){
        super.onResume();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                //随机生成新点
                PointValue value1=new PointValue(position*5,random.nextInt(20)+20);
                PointValue value2=new PointValue(position*5,random.nextInt(30)+30);
//                value1.setLabel("00:00");
//                value2.setLabel("00:00");
                WatPointValueList.add(value1);
                TemPointValueList.add(value2);
                float x=Math.max(value1.getX(),value2.getX());
                //根据新点画线
                lines.clear();
                Line TemLine=new Line(TemPointValueList);
                TemLine.setColor(Color.RED);//折线颜色
                TemLine.setCubic(true);//折线是平滑还是直线
//                TemLine.setShape(ValueShape.CIRCLE);//折线上点的形状
                TemLine.setHasLabelsOnlyForSelected(true);//点击点显示数据
                TemLine.setHasPoints(false);//是否显示圆点 如果为false 则没有原点只有点显示（每个数据点都是个大的圆点）
                lines.add(TemLine);

                Line WatLine=new Line(WatPointValueList);
                WatLine.setColor(Color.BLUE);
                WatLine.setCubic(true);
//                WatLine.setShape(ValueShape.CIRCLE);
                WatLine.setHasLabelsOnlyForSelected(true);
                WatLine.setHasPoints(false);
                lines.add(WatLine);

                data =initDatas(lines);
                lineChartView.setLineChartData(data);
                //根据横坐标变换界面显示范围
                Viewport port;
                if(x>50){
                    port=initViewPort(x-50,x);
                }
                else
                {
                    port=initViewPort(0,50);
                }
                lineChartView.setCurrentViewport(port);

                Viewport maxport=initMaxViewPort(x);
                lineChartView.setMaximumViewport(maxport);
                position++;
            }
        },0,5000);
    }
    public void PrintHumitureLine() {
        lineChartView = (LineChartView) findViewById(R.id.humiture_chart);
        //坐标轴
        axisX.setHasTiltedLabels(true);  //X坐标轴字体是斜的显示还是直的，true是斜的显示
        axisX.setTextColor(Color.BLACK);  //设置字体颜色
        axisX.setName("时间/s");  //表格名称
        axisX.setTextSize(10);//设置字体大小
        data.setAxisXBottom(axisX); //x 轴在底部
        axisX.setHasLines(true); //x 轴分割线

        // Y轴是根据数据的大小自动设置Y轴上限(在下面我会给出固定Y轴数据个数的解决方案)
        axisY.setName("温度/湿度");//y轴标注
        axisY.setTextColor(Color.BLACK);
        axisY.setTextSize(10);//设置字体大小
        data.setAxisYLeft(axisY);  //Y轴设置在左边
        axisY.setHasLines(true);

        data = initDatas(null);

        lineChartView.setLineChartData(data);

        Viewport viewport = initViewPort(0, 50);
        lineChartView.setCurrentViewportWithAnimation(viewport);
        lineChartView.setInteractive(false);
        lineChartView.setScrollEnabled(true);
        lineChartView.setValueTouchEnabled(true);
        lineChartView.setFocusableInTouchMode(true);
        lineChartView.setViewportCalculationEnabled(false);
        lineChartView.setContainerScrollEnabled(true, ContainerScrollType.HORIZONTAL);
        lineChartView.startDataAnimation();
        points = new ArrayList<>();
    }

    private LineChartData initDatas(List<Line> line)
    {
        LineChartData lineChartData=new LineChartData(line);
        lineChartData.setAxisYLeft(axisY);
        lineChartData.setAxisXBottom(axisX);
        return lineChartData;
    }

    //当前显示界面区域
    private Viewport initViewPort(float left,float right)
    {
        Viewport port=new Viewport();
        port.top=60;
        port.bottom=0;
        port.left=left;
        port.right=right;
        return port;
    }

    //最大显示区域
    private Viewport initMaxViewPort(float right)
    {
        Viewport port=new Viewport();
        port.top=60;
        port.bottom=0;
        port.left=0;
        port.right=right+50;
        return port;
    }




}