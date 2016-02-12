<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\Entity\RedhenContactType.
 */

namespace Drupal\redhen_contact\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_contact\RedhenContactTypeInterface;

/**
 * Defines the Contact type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_contact_type",
 *   label = @Translation("Contact type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_contact\RedhenContactTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_contact\Form\RedhenContactTypeForm",
 *       "edit" = "Drupal\redhen_contact\Form\RedhenContactTypeForm",
 *       "delete" = "Drupal\redhen_contact\Form\RedhenContactTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_contact\RedhenContactTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_contact_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_contact",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/contact_type/{redhen_contact_type}",
 *     "add-form" = "/admin/structure/redhen/contact_type/add",
 *     "edit-form" = "/admin/structure/redhen/contact_type/{redhen_contact_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/contact_type/{redhen_contact_type}/delete",
 *     "collection" = "/admin/structure/redhen/contact_type"
 *   }
 * )
 */
class RedhenContactType extends ConfigEntityBundleBase implements RedhenContactTypeInterface {
  /**
   * The Contact type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Contact type label.
   *
   * @var string
   */
  protected $label;

}
