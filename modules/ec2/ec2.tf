resource "aws_instance" "web" {
  #checkov:skip=CKV_AWS_79:Metadata of EC2 Instance not required
  
  ami                    = var.AMI
  instance_type          = var.instance_type
  key_name               = "${var.name}-Instance"
  vpc_security_group_ids = var.aws_security_group
  
  root_block_device {
      encrypted = true
  }
  
  #checkov:skip=CKV_AWS_135:EC2 is not EBS optimized 
  monitoring             = true   
  iam_instance_profile   = var.iam_instance_profile

   tags = {
    Name = "${var.name}-Instance"
    POD  = var.POD
  }
}
