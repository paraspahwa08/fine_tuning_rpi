output "sg_id" {
  description = "ID of Security Group"
  value       = aws_security_group.allow_sg.id
}
