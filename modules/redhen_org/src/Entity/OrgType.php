<?php

namespace Drupal\redhen_org\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_org\OrgTypeInterface;

/**
 * Defines the Organization type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_org_type",
 *   label = @Translation("Organization type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_org\OrgTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_org\Form\OrgTypeForm",
 *       "edit" = "Drupal\redhen_org\Form\OrgTypeForm",
 *       "delete" = "Drupal\redhen_org\Form\OrgTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_org\OrgTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_org_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_org",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/org_type/{redhen_org_type}",
 *     "add-form" = "/admin/structure/redhen/org_type/add",
 *     "edit-form" = "/admin/structure/redhen/org_type/{redhen_org_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/org_type/{redhen_org_type}/delete",
 *     "collection" = "/admin/structure/redhen/org_type"
 *   },
 *   config_export = {
 *     "uuid",
 *     "status",
 *     "id",
 *     "label",
 *   }
 * )
 */
class OrgType extends ConfigEntityBundleBase implements OrgTypeInterface {
  /**
   * The Org type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Org type label.
   *
   * @var string
   */
  protected $label;

}
