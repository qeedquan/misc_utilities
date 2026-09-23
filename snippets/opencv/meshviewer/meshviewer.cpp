#include <cstdio>
#include <iostream>
#include <vector>
#include <opencv2/viz.hpp>

using namespace cv;
using namespace std;

static void help()
{
	cout
		<< "--------------------------------------------------------------------------" << endl
		<< "This program shows how to use makeTransformToGlobal() to compute required pose,"
		<< "how to use makeCameraPose and Viz3d::setViewerPose. You can observe the scene "
		<< "from camera point of view (C) or global point of view (G)" << endl
		<< "Usage:" << endl
		<< "./transformations [ G | C ]" << endl
		<< endl;
}

static Mat cvcloud_load()
{
	auto fp = fopen("bunny.ply", "rb");
	if (!fp)
		return Mat();

	char line[1024];
	vector<Point3f> points;
	while (fgets(line, sizeof(line), fp))
	{
		Point3f point;
		float dummy1, dummy2;

		if (sscanf(line, "%f %f %f %f %f", &point.x, &point.y, &point.z, &dummy1, &dummy2) != 5)
			continue;

		points.push_back(point);
	}

	Mat cloud(1, points.size(), CV_32FC3);
	auto data = cloud.ptr<cv::Point3f>();
	memcpy(&data[0], &points[0], sizeof(points[0]) * points.size());
	cloud *= 5.0f;

	fclose(fp);
	return cloud;
}

int main(int argn, char **argv)
{
	help();
	if (argn < 2)
	{
		cout << "Missing arguments." << endl;
		return 1;
	}

	bool camera_pov = (argv[1][0] == 'C');
	viz::Viz3d myWindow("Coordinate Frame");
	myWindow.showWidget("Coordinate Widget", viz::WCoordinateSystem());

	Vec3f cam_pos(3.0f, 3.0f, 3.0f), cam_focal_point(3.0f, 3.0f, 2.0f), cam_y_dir(-1.0f, 0.0f, 0.0f);
	Affine3f cam_pose = viz::makeCameraPose(cam_pos, cam_focal_point, cam_y_dir);
	Affine3f transform = viz::makeTransformToGlobal(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f), cam_pos);

	Mat bunny_cloud = cvcloud_load();

	viz::WCloud cloud_widget(bunny_cloud, viz::Color::green());
	Affine3f cloud_pose = Affine3f().translate(Vec3f(0.0f, 0.0f, 3.0f));
	Affine3f cloud_pose_global = transform * cloud_pose;

	if (!camera_pov)
	{
		viz::WCameraPosition cpw(0.5); // Coordinate axes
		viz::WCameraPosition cpw_frustum(Vec2f(0.889484, 0.523599)); // Camera frustum
		myWindow.showWidget("CPW", cpw, cam_pose);
		myWindow.showWidget("CPW_FRUSTUM", cpw_frustum, cam_pose);
	}

	myWindow.showWidget("bunny", cloud_widget, cloud_pose_global);
	if (camera_pov)
		myWindow.setViewerPose(cam_pose);
	myWindow.spin();

	return 0;
}
