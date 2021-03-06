/*
Author: Katrina Santillan
Date: 15 May 2018
Robotics and Automation: Object Transformation Assignment

*/

#include "stdafx.h"
#include <opencv2\opencv.hpp>

void convert1(std::vector<cv::Point>& src, std::vector<cv::Point2f>& dst);


int main()
{
	cv::Mat image = cv::imread("../Reference Image.png", cv::IMREAD_GRAYSCALE);
	cv::Mat working_image, working_image1;
	cv::Size image_size;

	if (image.empty())
	{
		std::cout << "Could not find or open image" << std::endl;
	}

	cv::Mat blur, threshold, edges;
	cv::GaussianBlur(
		image,
		blur,
		cv::Size(5, 5),
		0
	);

	cv::threshold(
		blur,
		threshold,
		0,
		255,
		cv::THRESH_OTSU | cv::THRESH_BINARY
	);

	//Detect edges using Canny
	int threshold_ratio = 2;			//set threshold ratio, recommended 2:1 or 3:1
	double low_threshold = 10;			//set low threshold value
	double high_threshold = low_threshold * threshold_ratio;
	cv::Canny(threshold, edges, low_threshold, high_threshold);

	//Find contours
	std::vector<std::vector<cv::Point>> contour;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(
		edges,						//input
		contour,					//detected contours, stored as a vector of points
		hierarchy,					//image topology
		cv::RetrievalModes::RETR_CCOMP,							//retrieves all of the contours and organizes them into a two-level hierarchy
		cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE		//compresses segments and leaves only their end points, eg rectangle contour encoded with 4 points
	);

	//Find approximate polygon for each contour and draw it
	cv::RNG rng(12345);
	cv::Mat draw_contours = cv::Mat::zeros(edges.size(), CV_8UC3);
	std::vector<cv::RotatedRect> minRect(contour.size());
	std::vector<cv::Point> org_points, transform_points;


	//draws approx poly for org shape
	for (int i = 0; i < contour.size(); i++)
	{
		minRect[i] = cv::minAreaRect(cv::Mat(contour[i]));
		approxPolyDP(cv::Mat(contour[i]), contour[i], 3, true);
		//std::cout << contour[i] << " i: " << i << std::endl;
		cv::Scalar colour = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::drawContours(draw_contours, contour, i, colour, 1);				//draws approx polygon
		cv::Point2f rect_points[4];
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
		{
			//draws rotated bounding rectangle
			cv::line(draw_contours, rect_points[j], rect_points[(j + 1) % 4], colour);
		}
	}

	int x, y;
	for (x = 0; x < contour.size() / 2; x++)
	{
		for (int i = 0; i < contour[i].size(); i++)
		{
			org_points.push_back(contour[x][i]);
		}
	}

	for (int n = 0; n < contour.size(); n++)
	{
		y = x;						//store current x value
		while (contour[n].size() != contour[x].size())		//contour[n] = org shape, contour[x] transform shape
		{
			x++;											//look for a match in contour size
		}
		for (int i = 0; i < contour[i].size(); i++)
		{
			transform_points.push_back(contour[n][i]);
		}
	
		x = y;						//restore x value
	}

	std::vector<cv::Point2f> org_point2f, transform_point2f;
	convert1(org_points, org_point2f);
	convert1(transform_points, transform_point2f);

	cv::Mat homography = cv::findHomography(org_point2f, transform_points, cv::RANSAC);
	cv::warpPerspective(image, working_image, homography, working_image.size());

	cv::namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	cv::imshow("Contour", draw_contours);

	cv::namedWindow("Warped", CV_WINDOW_AUTOSIZE);
	cv::imshow("Warped", working_image);

	//cv::namedWindow("Contour Poly", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Contour Poly", draw_contours_poly);

	cv::waitKey(0);
}

void convert1(std::vector<cv::Point>& src, std::vector<cv::Point2f>& dst) {
	cv::Mat srcMat = cv::Mat(src);
	cv::Mat dstMat = cv::Mat(dst);
	cv::Mat tmpMat;
	srcMat.convertTo(tmpMat, dstMat.type());
	dst = (std::vector<cv::Point2f>) tmpMat;
}