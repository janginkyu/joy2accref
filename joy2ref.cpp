#include <chrono>
#include <memory>
#include <Eigen/Dense>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time_source.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "pcbf_msgs/msg/vector_xd.hpp"

#include <math.h>

using std::placeholders::_1;
using namespace std::chrono_literals;

class joy2refConvertor : public rclcpp::Node
{
public:
    joy2refConvertor()
    : Node("joy2accref")
    {
        publisher_ = this->create_publisher<pcbf_msgs::msg::VectorXd>("ref_input",10);
        timer_ = this->create_wall_timer(
            100ms, std::bind(&joy2refConvertor::accref_callback, this));

        subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", 10, std::bind(&joy2refConvertor::joy_callback, this, _1)
        );
    }

private:
    double acc_ref_ = 0.0;
    double angacc_ref_ = 0.0;
    bool stop_ = 0;
    
    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
    {
        double a = msg->axes[1];
        double b = msg->axes[0];

        acc_ref_ = 0.1 * a;
        angacc_ref_ = 0.5 * b;
        
        stop_ = msg->buttons[2];
    }
    void accref_callback()
    {
        // auto message = std_msgs::msg::Float64MultiArray();
        // std_msgs::msg::Float64MultiArray message;
        pcbf_msgs::msg::VectorXd message;
        message.header.stamp = this->get_clock()->now();
        if(!stop_) {
            message.data.push_back(0);
            message.data.push_back(0);
            message.data[0] = acc_ref_;
            message.data[1] = angacc_ref_;
        }
        // std::this_thread::sleep_for(1s);
        // message.data = "Acc is " + std::to_string(acc_ref_) + ", Angular acc is " + std::to_string(angacc_ref_);
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<pcbf_msgs::msg::VectorXd>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};



int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<joy2refConvertor>());
  rclcpp::shutdown();
  return 0;
}
