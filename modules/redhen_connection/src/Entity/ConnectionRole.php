<?php

namespace Drupal\redhen_connection\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\redhen_connection\ConnectionRoleInterface;

/**
 * Defines the Connection Role entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_connection_role",
 *   label = @Translation("Connection Role"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_connection\ConnectionRoleListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_connection\Form\ConnectionRoleForm",
 *       "edit" = "Drupal\redhen_connection\Form\ConnectionRoleForm",
 *       "delete" = "Drupal\redhen_connection\Form\ConnectionRoleDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_connection\ConnectionRoleHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_connection_role",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/connection-role/{redhen_connection_role}",
 *     "add-form" = "/admin/structure/redhen/connection-role/add",
 *     "edit-form" = "/admin/structure/redhen/connection-role/{redhen_connection_role}/edit",
 *     "delete-form" = "/admin/structure/redhen/connection-role/{redhen_connection_role}/delete",
 *     "collection" = "/admin/structure/redhen/connection-role"
 *   }
 * )
 */
class ConnectionRole extends ConfigEntityBase implements ConnectionRoleInterface {
  /**
   * The Connection Role ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Connection Role label.
   *
   * @var string
   */
  protected $label;

}
