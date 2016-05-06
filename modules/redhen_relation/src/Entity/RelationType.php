<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Entity\RelationType.
 */

namespace Drupal\redhen_relation\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_relation\RelationTypeInterface;

/**
 * Defines the Relation type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_relation_type",
 *   label = @Translation("Relation type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_relation\RelationTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_relation\Form\RelationTypeForm",
 *       "edit" = "Drupal\redhen_relation\Form\RelationTypeForm",
 *       "delete" = "Drupal\redhen_relation\Form\RelationTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_relation\RelationTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_relation_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_relation",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/relation_type/{redhen_relation_type}",
 *     "add-form" = "/admin/structure/redhen/relation_type/add",
 *     "edit-form" = "/admin/structure/redhen/relation_type/{redhen_relation_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/relation_type/{redhen_relation_type}/delete",
 *     "collection" = "/admin/structure/redhen/relation_type"
 *   }
 * )
 */
class RelationType extends ConfigEntityBundleBase implements RelationTypeInterface {
  /**
   * The Relation type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Relation type label.
   *
   * @var string
   */
  protected $label;

}
