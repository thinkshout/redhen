<?php

/**
 * @file
 * Contains \Drupal\redhen_connection\Entity\ConnectionType.
 */

namespace Drupal\redhen_connection\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_connection\ConnectionTypeInterface;

/**
 * Defines the Connection type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_connection_type",
 *   label = @Translation("Connection type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_connection\ConnectionTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_connection\Form\ConnectionTypeForm",
 *       "edit" = "Drupal\redhen_connection\Form\ConnectionTypeForm",
 *       "delete" = "Drupal\redhen_connection\Form\ConnectionTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_connection\ConnectionTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_connection_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_connection",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/connection_type/{redhen_connection_type}",
 *     "add-form" = "/admin/structure/redhen/connection_type/add",
 *     "edit-form" = "/admin/structure/redhen/connection_type/{redhen_connection_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/connection_type/{redhen_connection_type}/delete",
 *     "collection" = "/admin/structure/redhen/connection_type"
 *   }
 * )
 */
class ConnectionType extends ConfigEntityBundleBase implements ConnectionTypeInterface {
  /**
   * The Connection type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Connection type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The first entity type ID.
   *
   * @var string
   */
  protected $entityType1;

  /**
   * The second entity type ID.
   *
   * @var string
   */
  protected $entityType2;

  /**
   * {@inheritdoc}
   */
  public function getEndpointEntityTypeId($field) {
    $field = 'entityType' . $field;
    return $this->{$field};
  }
  /**
   * {@inheritdoc}
   */
  public function setEndpointEntityTypeId($field, $entity_type_id) {
    $field = 'entityType' . $field;
    $this->{$field} = $entity_type_id;
    return $this;
  }

}
