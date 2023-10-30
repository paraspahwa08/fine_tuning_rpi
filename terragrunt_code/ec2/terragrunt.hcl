include "root" {
  path = find_in_parent_folders("terragrunt.hcl")
}

terraform {
  #source = "git@github.com:Deloitte/PES-HMI-Terraform.git//modules/ec2?ref=master"
  #source = "/home/ubuntu/actions-runner/_work/PES-HMI-Terraform/PES-HMI-Terraform/modules//ec2"
  source  = "/home/runner/work/Telematics-Rpi/Telematics-Rpi/modules//ec2"
}

dependency "security_group" {
  config_path  = "../security_group"
  mock_outputs = {
    sg_id = "sample-sg"
  }
}

dependency "iam_instance_profile" {
  config_path = "../iam_instance_profile"
  mock_outputs = {
     instance_profile_name = "sample-instance-profile-name"
  }
}


# Indicate the input values to use for the variables of the module.
inputs = {
      name               = "SonarQube"
      POD                = "Telematics"
      AMI                = "ami-0cd7ad8676931d727"
      instance_type      = "t2.medium"
      aws_security_group = [dependency.security_group.outputs.sg_id]
      iam_instance_profile = dependency.iam_instance_profile.outputs.instance_profile_name
}
